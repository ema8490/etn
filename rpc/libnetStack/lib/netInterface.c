//______________________________________________________________________________
/// netInterface generic routines
//
//  Externally callable routines:
//     netInterfaceBlocked
//     netInterfaceProcess
//     netInterfaceSend
//     netInterfaceSendFull
//     netInterfaceInit: generic version, called by non-generic versions
//
//  Aug-2009: Jon A. Solworth
//______________________________________________________________________________

#include <rpcKernel.h>
#include <ethos/generic/debug.h>
#include <ethos/generic/hashTable.h>
#include <ethos/dual/core.h>
#include <ethos/generic/tunnel.h>
#include <ethos/generic/netInterface.h>
#include <ethos/generic/ethernet.h>
#include <ethos/generic/connection.h>
#include <ethos/generic/envelope.h>

NetInterface netInterface[MaxInterfaces];
Tunnel      *shadowdaemonTunnel;
Tunnel      *terminalTunnel;

uint         nextInterfaceNumber = 0;

//______________________________________________________________________________
// get net interface
//______________________________________________________________________________
NetInterface*
netInterfaceGet(uint interfaceNumber)
{
    if (!netInterfaceValid(interfaceNumber))
	{
	    debugXPrint(tunnelDebug, "invalid interfaceNumber = $[int]\n",
		     interfaceNumber);
	    return NULL;
	}
    return netInterface + interfaceNumber;
}

//______________________________________________________________________________
// check that the interface number corresponds to existing interface
//______________________________________________________________________________
bool
netInterfaceValid(uint interfaceNumber)
{
    return interfaceNumber < nextInterfaceNumber;
}


//______________________________________________________________________________
/// accept incoming packets, allowing Dom0 to complete sending arbitrary
/// amounts of data, and then process them to effect events.   This routine
/// can only be called at quiescent times (during a context switch).
//______________________________________________________________________________
int
netInterfaceDo(NetInterface *ni)
{
    ASSERT(ni);
    int anything; // anything happen on this iteration?
    do
	{
	    anything   = netInterfaceProcess(ni);
	    anything  |= netInterfacePending(ni);
	} while (anything);
    return anything;
}

//______________________________________________________________________________
// processes incoming packets on all interfaces
//______________________________________________________________________________
int
netInterfaceDoAll(void)
{
    int i,j;
    int anything = 1;
    for (i=0; anything; i++)
	{
	    anything = 0;
	    for (j=0; j<nextInterfaceNumber; j++)
		{
		    anything |= netInterfaceDo(netInterface + j);
		}
	}
    return i-1;
}

//______________________________________________________________________________
// Get a tunnel ID from a Tunnel list entry
//______________________________________________________________________________
static
HashKey
_getTunnelId (ListHead *element)
{
    ASSERT (element);

    return (HashKey) list_entry(element, Tunnel, tunnelIdList)->tunnelId;
}

//______________________________________________________________________________
// Compare a tunnel ID key to a Tunnel list entry's tunnel ID
//______________________________________________________________________________
static
int
_compareTunnelId (ListHead *element, HashKey key)
{
    ASSERT (element);
    ASSERT (key);

    return memcmp (list_entry(element, Tunnel, tunnelIdList)->tunnelId, key, TunnelIdSize);
}

//______________________________________________________________________________
// Get a public key from a Tunnel list entry
//______________________________________________________________________________
static
HashKey
_getPublicKey (ListHead *element)
{
    ASSERT (element);

    return (HashKey) list_entry(element, Tunnel, tunnelPublicKeyList)->envelopeRemote.ephemeralPublicKey;
}

//______________________________________________________________________________
// Compare a public key to a Tunnel list entry's public key
//______________________________________________________________________________
static
int
_comparePublicKey (ListHead *element, HashKey key)
{
    ASSERT (element);
    ASSERT (key);

    return memcmp (list_entry(element, Tunnel, tunnelPublicKeyList)->envelopeRemote.ephemeralPublicKey, key, CryptoBoxPublicKeySize);
}


//______________________________________________________________________________
// Get a hostname from a Tunnel list entry
//______________________________________________________________________________
static
HashKey
_getHostname (ListHead *element)
{
    ASSERT (element);

    return (HashKey) list_entry(element, Tunnel, tunnelHostnameList)->envelopeRemote.hostname;
}

//______________________________________________________________________________
// Compare a hostname key to a Tunnel list entry's hostname
//______________________________________________________________________________
static
int
_compareHostname (ListHead *element, HashKey _key)
{
    ASSERT (element);
    ASSERT (_key);

    String *key = (String *) _key;
    String *hostname = list_entry(element, Tunnel, tunnelHostnameList)->envelopeRemote.hostname;
    if (NULL == hostname && NULL == key)
	{
	    return 0;
	}
    else if (NULL == hostname)
	{
	    return -1;
	}
    else if (NULL == hostname)
	{
	    return 1;
	}
    else
	{
	    return stringCompare (hostname, key);
	}
}

//______________________________________________________________________________
// Create the network header table to calculate the header offsets, header sizes,
// and maximum payload size. 
//______________________________________________________________________________
NetInterface *
netInterfaceInit(NetInterfaceType netInterfaceType,
		 uint headers,
		 EnvelopeLocal *el,
		 EtnInterfaceType *defaultRpcInterface)
{
    ASSERT(nextInterfaceNumber < MaxInterfaces);
    uint interfaceNumber = nextInterfaceNumber++;

    NetInterface *ni = netInterface + interfaceNumber;

    ni->interfaceNumber          = interfaceNumber;     // multiple interfaces
    ni->netInterfaceType         = netInterfaceType;    // netInterface type
    ni->defaultRpcInterface      = defaultRpcInterface; // default RPC interface
    ni->headers                  = headers;
    ni->headerSize               = 0;
    ni->reserveSize              = 0;
    ni->otherSize                = 0;
    ni->envelopeLocal            = *el;
    ni->tunnelIdHashTable        = hashTableCreate(_getTunnelId,  _compareTunnelId);
    ni->tunnelPublicKeyHashTable = hashTableCreate(_getPublicKey, _comparePublicKey);
    ni->tunnelHostnameHashTable  = hashTableCreate(_getHostname,  _compareHostname);

    INIT_LIST_HEAD(&ni->pendingIn);
    INIT_LIST_HEAD(&ni->expiredRwSlots);              
    INIT_LIST_HEAD(&ni->tunnelHasPendingOut);              

    envelopeInit(ni);

    return ni;
}
    


//______________________________________________________________________________
///	1. preprocess all incoming, remove from ring buffer, and adjust
///        outgoing window
///	2. retransmit packets whose timer has expired timer expired
///	3. copy pending to outgoing window (and transmit) if room
//______________________________________________________________________________
int
netInterfacePending(NetInterface *ni)
{
    ASSERT(ni);

    int anything = 0;
    reliabilityWindowDo();
    anything |= netInterfaceIncoming(ni);          // process incoming window
    anything |= tunnelResend(&ni->expiredRwSlots); // resend all packets which have timed out

    ListHead *v, *tmp;
    list_for_each_safe(v, tmp, &ni->tunnelHasPendingOut)
	{
	    Tunnel *tunnel = list_entry(v, Tunnel, tunnelHasPendingOut);
	    anything |= tunnelSendBacklog(tunnel);    // if room in send window, send more
	    list_del_init(&tunnel->tunnelHasPendingOut);
	}

    return anything;
}

void
netInterfacePrintExpired(char *where, NetInterface *ni)
{
    debugXPrint(tunnelDebug, "$[cstring]: expiredRwSlots for ni $[int]",
	     where, ni->interfaceNumber);
    ListHead *v;
    list_for_each(v, &ni->expiredRwSlots)
	{
	    Packet *p = list_entry(v, ReliabilityWindow, retransmitList)->packet;
	    packetPrint("--",p);
	}
    debugXPrint(tunnelDebug, "\n");
}

//______________________________________________________________________________
/// Process incoming packets.  This should be done only when kernel is quiescent
/// to prevent race conditions.
//______________________________________________________________________________
int
netInterfaceProcess(NetInterface *ni)
{
    ASSERT(ni);
    int anything = 0;
    // walk through the entire pendingIn list for the netInterface
    // processing each packet
    ListHead *q, *tmp;
    list_for_each_safe(q, tmp, &ni->pendingIn)
	{
	    Packet *packet = (Packet *) list_entry(q, Packet, _packetList);
	    ASSERT(packet);
	    tunnelProcessInPacket(packet);
	    packetRemoveFromList(packet);
	    packetFree(packet);
	    anything++;
	}

    return anything;
}

//______________________________________________________________________________
/// Initial receive of packet
///   initial tunnel processing has already been done
///   (lost, corrupted, duplicate, or out-of-order arrival)
///   so that only enqueuing is needed here.
//______________________________________________________________________________
void
netInterfaceReceivePacket(Packet *packet)
{
    ASSERT(packet);
    Tunnel             *tunnel = packet->tunnel;
    ASSERT(tunnel);
    NetInterface *ni = tunnel->netInterface;
    ASSERT(ni);

    // add to end of pendingIn
    list_add_tail(&packet->_packetList, &ni->pendingIn); 

    bool sent = false;
    packet = tunnelIsWaiting(packet);
    while (packet)
	{
	    list_add_tail(&packet->_packetList, &ni->pendingIn); 
	    sent = true;
	    packet = tunnelIsWaiting(packet);
	}
    if (sent)
	{
	    tunnelSendBacklog(tunnel);
	}

}
