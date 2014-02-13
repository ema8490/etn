//______________________________________________________________________________
// tunnel containing multiple connections
//
// Aug-2009: Jon A. Solworth
//______________________________________________________________________________

#include <rpcKernel.h>
#include <ethos/generic/assert.h>
#include <ethos/generic/debug.h>
#include <ethos/generic/hashTable.h>
#include <ethos/dual/status.h>
#include <ethos/dual/core.h>
#include <ethos/generic/tunnel.h>
#include <ethos/generic/connection.h>
#include <ethos/generic/netInterface.h>
#include <ethos/generic/packet.h>
#include <ethos/generic/reliabilityWindow.h>
#include <ethos/dual/xalloc.h>


static uint tunnelXtype;

//______________________________________________________________________________
//     Tunnel manages point-to-point communications, and ensures reliable packet
//     semantics.
//______________________________________________________________________________

enum PacketDataEnum {PacketEmpty, PacketNotEmpty};
typedef enum PacketDataEnum PacketData;
static bool _tunnelSend(Packet *packet, PacketData packetData);

//______________________________________________________________________________
/// returns a value between 0...ReliabilityWindowSize
//______________________________________________________________________________
static
inline
uint32
_tunnelReliabilityWindowDiff(uint32 diff)
{
    if (diff > ReliabilityWindowSize)
	return 0;
    else
	return diff;
}

//______________________________________________________________________________
// default tunnel initialization
//______________________________________________________________________________
static
void
_tunnelInitialize(void *ptr)
{
    Tunnel *tunnel = (Tunnel *) ptr;
    memzero((void *)tunnel, sizeof(Tunnel));

    INIT_LIST_HEAD(&tunnel->pendingOut);
    INIT_LIST_HEAD(&tunnel->tunnelHasPendingOut);

    int i;
    for (i=0; i<ReliabilityWindowSize; i++)
	{
	    INIT_LIST_HEAD(&tunnel->reliabilityWindow[i].retransmitList);
	    tunnel->reliabilityWindow[i].packet = NULL;
	    tunnel->incoming[i] = NULL;
	}

}

//______________________________________________________________________________
// set up tunnel allocator
//______________________________________________________________________________
void
tunnelInit(void)
{
    tunnelXtype = xtype("Tunnel", sizeof(Tunnel), NULL, _tunnelInitialize, NULL);
    REQUIRE(tunnelXtype);
}

//______________________________________________________________________________
/// allocate a new tunnel
//______________________________________________________________________________
static
Tunnel *
_tunnelAllocate(NetInterface *ni, EnvelopeRemote *er, EtnInterfaceType *rpcInterface, bool isOutgoing)
{
    debugXPrint(tunnelDebug, "entering\n");
    ASSERT(ni);
    ASSERT(ni->tunnelIdHashTable);
    ASSERT(ni->tunnelPublicKeyHashTable);
    ASSERT(ni->tunnelHostnameHashTable);
    ASSERT(!ni->locked);

    Tunnel *tunnel = NULL;

    EnvelopeLocal  *el =  &ni->envelopeLocal;
    //er->nonce = 0;

    if (ni->headers & HeaderCrypto)
	{
	    if (false == cryptoComputeSymmetricKey(el, er, isOutgoing ? NetCryptoKeyEphemeralOutgoing : NetCryptoKeyEphemeralIncoming))
		{
		    goto done;
		}
	}
    else
	{
	    ASSERT(! ((el->ipAddress == er->ipAddress) && (el->udpPort == er->udpPort)));

	    er->oddSide = (el->ipAddress > er->ipAddress) || 
	                 ((el->ipAddress == er->ipAddress) && (el->udpPort > er->udpPort));
	}

    ASSERT(tunnelXtype);
    tunnel = (Tunnel *) xalloc(tunnelXtype, 1);
    ASSERT(tunnel);

    // tunnel->maximumPayloadSize could be increased a bit
    // if no crypto headers, but note we're assuming Ethernet/IP/UDP
    tunnel->netInterface = ni;
    tunnel->controlConnection = NULL;
    tunnel->maximumPayloadSize = EthernetMaxBufferSize - MaxHeaderSize;

    rttInit(&tunnel->rtt);

    envelopeGetTunnelId(tunnel->tunnelId, ni->headers, el, er);

    tunnel->envelopeRemote = *er;
    tunnel->rpcInterface   =  rpcInterface;

    bool ok;
    INIT_LIST_HEAD(&tunnel->tunnelIdList);
    ok = hashTableInsert(ni->tunnelIdHashTable, &tunnel->tunnelIdList);
    ASSERT(true == ok);

    INIT_LIST_HEAD(&tunnel->tunnelPublicKeyList);
    ok = hashTableInsert(ni->tunnelPublicKeyHashTable, &tunnel->tunnelPublicKeyList);
    ASSERT(true == ok);

    INIT_LIST_HEAD(&tunnel->tunnelHostnameList);
    if (NULL != tunnel->envelopeRemote.hostname)
	{
	    ok = hashTableInsert(ni->tunnelHostnameHashTable, &tunnel->tunnelHostnameList);
	    ASSERT(true == ok);
	}

done:
    return tunnel;
}


//______________________________________________________________________________
/// intitialize an ethernet tunnel
//______________________________________________________________________________
Tunnel *
tunnelCreate(NetInterface   *ni,
	     EnvelopeRemote *er,
             EtnInterfaceType *rpcInterface,
             bool isOutgoing
	     )
{
    ASSERT(ni);
    ASSERT(ni->tunnelIdHashTable);
    // FIXME:
    // If not using HeaderCrypto or HeaderEthernet, then there can be only one tunnel on
    // an interface.  This is because tunnelId's are based on cryptoTunnelId or MAC
    // addresses (see envelopeGetTunnelId). The Posix networking-based library is an
    // example of absent Ethernet headers; furthermore, in this case crypto headers are
    // not used when talking to shadowdaemon.
    ASSERT(ni->headers & HeaderCrypto
           || ni->headers & HeaderEthernet
	   || ni->tunnelIdHashTable->elements == 0);

    Tunnel *tunnel = _tunnelAllocate(ni,er,rpcInterface,isOutgoing);
    if (NULL == tunnel)
	{
	    goto done;
	}

    tunnel->envelopeRemote.nonce = 0;

    debugXPrint(tunnelDebug, "ni->interfaceNumber = $[int]   ni->netInterfaceType = $[int]"
		"   ni->headers = $[int]   oddSide=$[int]   oddSide=$[int]\n",
		ni->interfaceNumber, ni->netInterfaceType,
		ni->headers, tunnel->envelopeRemote.oddSide, tunnel->envelopeRemote.oddSide);

    envelopeLocalPrint(&ni->envelopeLocal);
    envelopeRemotePrint(&tunnel->envelopeRemote);

done:
    return tunnel;
}		     

//______________________________________________________________________________
/// free the tunnel
//______________________________________________________________________________
void
tunnelFree(Tunnel *tunnel)
{
    ASSERT(tunnel);
    xfree(tunnel);
}


//______________________________________________________________________________
/// create a packet
//______________________________________________________________________________
Packet*
tunnelPacketCreate(Tunnel *tunnel,
		   uint32 size  // size includes everything after reserved area
		   )
{
    ASSERT(tunnel);
    NetInterface *ni = tunnel->netInterface;
    ASSERT(ni);

    uint32 total = size + ni->reserveSize + ni->otherSize;
    if (total > tunnel->maximumPayloadSize)
	{
	    total = tunnel->maximumPayloadSize;
	}
    Packet *packet = packetAllocate(total);
    packet->tunnel = tunnel;

    // reliability headers
    packetSequenceAcknowledge(packet, tunnel->sequence, tunnel->acknowledge);
    tunnel->sequence++;
    return packet;
}

//______________________________________________________________________________
/// create a first packet
//______________________________________________________________________________
Packet*
tunnelPacketCreateFirst(Tunnel *tunnel,
		        uint32 size  // size includes everything after reserved area
		       )
{
    ASSERT(tunnel);
    NetInterface *ni = tunnel->netInterface;
    ASSERT(ni);

    uint32 total = size
                 + ni->reserveSize
                 + ni->otherSize;
    
    if (total > tunnel->maximumPayloadSize)
	{
	    total = tunnel->maximumPayloadSize;
	}

    Packet *packet      = packetAllocate(total);
    packet->tunnel      = tunnel;
    packet->firstPacket = true;

    // reliability headers
    packetSequenceAcknowledge(packet, tunnel->sequence, tunnel->acknowledge);
    tunnel->sequence++;

    return packet;
}

//______________________________________________________________________________
/// create an empty packet for acknowledgement, don't increment sequence number
//______________________________________________________________________________
Packet*
tunnelPacketCreateEmpty(Tunnel *tunnel)
{
    ASSERT(tunnel);
    NetInterface *ni = tunnel->netInterface;
    ASSERT(ni);

    Packet *packet = packetAllocate(ni->reserveSize + ni->otherSize);
    packet->tunnel = tunnel;

    // reliability headers, don't bump sequence number
    packetSequenceAcknowledge(packet, tunnel->sequence, tunnel->acknowledge);

    return packet;
}

//______________________________________________________________________________
///  In general, the maximum packet size depends on the tunnel, not the interface
//______________________________________________________________________________
Packet*
tunnelPacketCreateMax(Tunnel *tunnel)
{
    ASSERT(tunnel);
    return tunnelPacketCreate(tunnel, tunnel->maximumPayloadSize);
}


//______________________________________________________________________________
///  Increase the space of the number of connections
//______________________________________________________________________________
static
bool
_tunnelConnectionIncreaseSpace(Tunnel *tunnel)
{
    ASSERT(tunnel);

    int elmts = (tunnel->connectionAllocated == 0) 
	? 32 : tunnel->connectionAllocated*2;

    Connection **oldConnection = tunnel->connectionArray;
    int size = elmts * sizeof(Connection*);
    tunnel->connectionArray = malloc(size);
    if (NULL==tunnel->connectionArray)
	{
	    return false;
	}
    memzero(tunnel->connectionArray, size);
    memcpy(tunnel->connectionArray,
	   oldConnection,
	   tunnel->connectionAllocated * sizeof(Connection*));
    free(oldConnection);

    uint32 min = tunnel->envelopeRemote.oddSide ? 1 : 2;
    long i;
    long allocated = tunnel->connectionAllocated;
    for (i=elmts-min; i > allocated; i-=2)
	{   // connect up this side (either even or odd)
	    debugXPrint(tunnelDebug, "elmts is $[int]   odd is $[int]"
		     "   connection is $[int]    allocated is $[int]\n",
		     elmts, tunnel->envelopeRemote.oddSide, i, allocated);
	    ASSERT(mySideConnectionId(tunnel, i));

	    tunnel->connectionArray[i] =  (void*)tunnel->connectionFree;
	    tunnel->connectionFree = i;
	}
    debugXPrint(tunnelDebug, "loop finished\n");

    tunnel->connectionAllocated = elmts;

    return true;
}

//______________________________________________________________________________
///  Determine a connectionId on which to receive info
//______________________________________________________________________________
uint32
tunnelConnectionAllocate(Tunnel *tunnel)
{
    ASSERT(tunnel);
    bool canIncrease;

    if (0==tunnel->connectionFree) 
	{
	    canIncrease = _tunnelConnectionIncreaseSpace(tunnel);
	    if (!canIncrease)
		{
		    return 0;
		}
	}

    long alloc = tunnel->connectionFree;
    tunnel->connectionFree = (long)tunnel->connectionArray[alloc];
    
    ASSERT(alloc); // not zero!
    
    tunnel->connectionArray[alloc] = connectionAlloc();

    tunnel->connectionArray[alloc]->tunnel = tunnel;
    tunnel->connectionArray[alloc]->connectionId = alloc;
    tunnel->connectionArray[alloc]->connectionStatus = ConnectionInitiated;
    return alloc;
}

//______________________________________________________________________________
/// accept a new connection
//______________________________________________________________________________
static
Connection*
_tunnelConnectionAccept(Tunnel *tunnel, uint32 connectionId)
{
    debugXPrint(tunnelDebug, "connectionId = $[int]\n", connectionId);

    ASSERT(tunnel);
    ASSERT(!mySideConnectionId(tunnel, connectionId));
    bool canIncrease;

    if (connectionId >= tunnel->connectionAllocated)
	{
	    canIncrease = _tunnelConnectionIncreaseSpace(tunnel);
	    if (!canIncrease)
		{
		    return NULL;
		}
	}

    ASSERT(connectionId < tunnel->connectionAllocated);

    ASSERT(!tunnel->connectionArray[connectionId]);

    tunnel->connectionArray[connectionId] = connectionAlloc();

    tunnel->connectionArray[connectionId]->tunnel = tunnel;
    tunnel->connectionArray[connectionId]->connectionId = connectionId;
    tunnel->connectionArray[connectionId]->connectionStatus = ConnectionEstablished;

    return tunnel->connectionArray[connectionId];
}

bool tunnelConnectionIdValid(Tunnel *tunnel, uint32 connectionId) { return tunnel->connectionArray[connectionId]; }

//______________________________________________________________________________
///  release a connection Id so that it can be reallocated in the future
//______________________________________________________________________________
static
void
_tunnelConnectionRelease(Tunnel *tunnel, uint32 connectionId)
{
    ASSERT(tunnel);

    connectionFree(tunnel->connectionArray[connectionId]);

    if (mySideConnectionId(tunnel, connectionId))
	{   // put it on the free list
	    tunnel->connectionArray[connectionId] = (void*) tunnel->connectionFree;
	    tunnel->connectionFree = connectionId;
	}
    else
	{   // mark it as available
	    tunnel->connectionArray[connectionId] = 0;
	}
	    
}

char *connectionOp[8] =
    {
	"NewConnection",
	"AcceptConnection",
	"RejectConnection",
	"CloseConnection",
	"SendOnConnection",
	"SendOnConnectionF",
	"SendOnConenctionL",
	"SendOnConnectionFL"
    };


//______________________________________________________________________________
///  Process next packet for the tunnel, performing connection level protocol
///  including connection management and de-packetizing.
///  Packet has already been checked that it is to the right MAC address and
///  packet reliability headers have been processed.
//______________________________________________________________________________
void
tunnelProcessInPacket(Packet *packet)
{
    ASSERT(packet);
    Tunnel *tunnel = packet->tunnel;
    ASSERT(tunnel);

    debugXPrint(tunnelDebug, "In tunnelProcessInPacket\n");
    static int bytesRemaining;
    ASSERT(0!=remainingReadableBytes(packet));

    while (0!=remainingReadableBytes(packet))
	{
	    uint32 operation    = getUint32(packet);
	    uint32 connectionId = getUint32(packet);
	    Connection *connection = NULL;
	    uint32 op = operation >> connectionOperationShift;
	    uint32 bytes = operation & connectionSizeMask;

	    switch (op)
		{
		case NewConnection: // new connection, set outgoing channel
		    // XXXXXXXXXXX
		    if (connectionId==0)
			{ // 0 padded ethernet packet
			    return;
			}

		    debugXPrint(tunnelDebug, "NewConnection  connectionId = $[int]\n", connectionId);
		    connection  = _tunnelConnectionAccept(tunnel, connectionId);

		    // reply, accepting the new connection
		    acceptConnection(connection);
		    break;
		case AcceptConnection: // new connection, set outgoing channel

		    debugXPrint(tunnelDebug, "AcceptConnection  connectionId = $[int]\n", connectionId);

		    connection = tunnel->connectionArray[connectionId];
		    ASSERT(connection->connectionStatus == ConnectionInitiated);
		    connection->connectionStatus = ConnectionEstablished;
		    break;
		case RejectConnection:  // free up connection
		    _tunnelConnectionRelease(tunnel, connectionId);
		    break;
		case CloseConnection:
		    _tunnelConnectionRelease(tunnel, connectionId);
		    break;
		case SendOnConnectionF:
		case SendOnConnectionFL:
		case SendOnConnection:
		case SendOnConnectionL:
		    // size of allocation is in words
		    connection = tunnel->connectionArray[connectionId];
		    if ((SendOnConnectionF==op) || (SendOnConnectionFL==op))
			{
			    debugXPrint(tunnelDebug, "bytes needed = $[int]\n", bytesRemaining=bytes);
			    connection->inPacket = packetAllocate(bytes);
			}
		    if (bytes > remainingReadableBytes(packet))
			{  // no bigger than what remains in the in packet
			    bytes = remainingReadableBytes(packet);
			}

		    bytesRemaining -= bytes;
		    debugXPrint(tunnelDebug, "bytes remaining = $[int]    op = $[cstring]\n",
		             bytesRemaining, connectionOp[op]);

		    putBytes(packet, bytes, connection->inPacket);
		    if (operation & connectionLastPacket)
			{
			    debugXPrint(tunnelDebug, "last packet of connection\n");
			    connectionProcessPacket(connection, connection->inPacket);
			    packetRemoveFromList(connection->inPacket);
			    ASSERT(list_empty(&connection->inPacket->_packetList));
			    packetFree(connection->inPacket);
			    connection->inPacket = NULL;
			}
		}
	}
}

//______________________________________________________________________________
// tunnelIsWaiting
//______________________________________________________________________________
Packet *
tunnelIsWaiting(Packet *packet)
{
    ASSERT(packet);
    Tunnel *tunnel = packet->tunnel;
    ASSERT(tunnel);
    
    uint32 index = tunnel->acknowledge % ReliabilityWindowSize;
    packet = tunnel->incoming[index];
    if (packet)
	{
	    tunnel->incoming[index] = NULL;
	    tunnel->acknowledge++;
	}
    return packet;
}


//______________________________________________________________________________
///  tunnelIsNewPacket
///  is an incoming packet a new packet (for shadowdaemon or ethos kernel)
///  returns true iff
//        new and it contains a data
//______________________________________________________________________________
bool
tunnelIsNewPacket(Packet *packet)
{
    ASSERT(packet);
    Tunnel *tunnel = packet->tunnel;
    ASSERT(tunnel);

    // check sequence number
    uint32 sequence    = getUint32(packet);
    uint32 acknowledge = getUint32(packet); 

    uint32 bytes = remainingReadableBytes(packet); // 0 iff no data

    bool wasUnacked = tunnel->unackedPacket;
    tunnel->unackedPacket += bytes;  // ignore packets with no data
    if (list_empty(&tunnel->tunnelHasPendingOut))
	{
	    list_add_tail(&tunnel->tunnelHasPendingOut, &tunnel->netInterface->tunnelHasPendingOut); 
	}

    // adjust start of the send window
    uint32 acked = _tunnelReliabilityWindowDiff(acknowledge - tunnel->start);

    debugXPrint(tunnelDebug, "seq = $[uint]   ack = $[uint]   tunnel->start=$[uint]"
	     "   tunnel->ack=$[uint]  acked=$[uint]  diff=$[uint]   bytes = $[uint] remoteUdpPort = ($[uint])\n",
	     sequence, acknowledge, tunnel->start, tunnel->acknowledge, acked, acknowledge-tunnel->start, bytes, tunnel->envelopeRemote.udpPort);

    while (acked--)
	{
	    uint32 index = tunnel->start % ReliabilityWindowSize;
	    // If the packet is not a retransmitted packet, then the rto values have to be updated
	    // debugXPrint(tunnelDebug, "The resend count is  $[int]\n", tunnel->reliabilityWindow[index].transmitCount);
	    ReliabilityWindow *rw = &tunnel->reliabilityWindow[index];
	    if (1 == rw->transmitCount)  
		{
		    rttUpdateRto(&tunnel->rtt, rttCurrentTime() - rw->timestamp);
		    debugXPrint(tunnelDebug, "Now the RTO is $[lluint]ms\n", tunnel->rtt.rto);
		}
	    if (1==acknowledge)
		{   // first reply on a received packet, re-init rtt in case target host was down initially
		    rttInit(&tunnel->rtt);
		}

	    if (rw->packet)
		{ // was not a NoData packet
		    packetPrint("tunnelIsNewPacket: remove ", rw->packet);
		    ASSERT(list_empty(&rw->packet->_packetList));
		    packetFree(rw->packet);
		    rw->packet = NULL;
		    reliabilityWindowRemove(rw); // remove timer.
		}

	    ASSERT(list_empty(&rw->retransmitList));
	    
	    tunnel->start++;
	}

    if (0==bytes)
	{   // data empty packet, no more processing
	    return false;
	}

    // data in the packet
    if (sequence==tunnel->acknowledge)
	{   // got the next packet in order

	    tunnel->acknowledge++;
	    debugXPrint(tunnelDebug, "bumped tunnel->acknowledge=$[uint]\n", tunnel->acknowledge);
	    return true;
	}
    else if (sequence > tunnel->acknowledge)
	{
	    if ((sequence - tunnel->acknowledge) < ReliabilityWindowSize)
		{
		    uint32 index = sequence % ReliabilityWindowSize;
		    tunnel->incoming[index] = packetCopy(packet);
		}
	}
    else if ((sequence < tunnel->acknowledge) && wasUnacked && tunnel->start==tunnel->next)
	{ // we're seeing old packets and we have unacked receives, ack them
	    packet = tunnelPacketCreateEmpty(tunnel);
	    if (_tunnelSend(packet, PacketEmpty))
		{ 
		    tunnel->unackedPacket = 0;
		}
	    // empty packets not put on reliability window, sent and forgotten
	    packetFree(packet);
	}

    // out-of-order packet
    debugXPrint(tunnelDebug, "out-of-order packet (seq. = $[int]), prev. seq. = $[int]\n",
	      sequence,
	      tunnel->acknowledge);
    envelopeRemotePrint(&tunnel->envelopeRemote);

    return false;
}

//______________________________________________________________________________
///  Send a packet out an interface.  To actually send it out of the interface
///    1. the packet list for the tunnel must be empty
///    2. the window must have room in it for the packet
//______________________________________________________________________________
void
tunnelOut(Packet *packet)
{
    static uint32 count = 0;

    ASSERT(packet);
    Tunnel *tunnel = packet->tunnel;
    ASSERT(tunnel);
    NetInterface *ni = tunnel->netInterface;
    ASSERT(ni);


    count++;
    // make room for outgoing packets
    if (0==count%HalfReliabilityWindowSize)
	{  // send acks and expired packet
	    netInterfacePending(ni);
	}
    else
	{
	    netInterfaceIncoming(ni);
	}

    //packetPrint("tunnelOut", packet);

    tunnelSendBacklog(tunnel); 

    if (!list_empty(&tunnel->pendingOut) || !_tunnelSend(packet, PacketNotEmpty))
	{   // failed to send right now, enqueue it
	    list_add_tail(&packet->_packetList, &tunnel->pendingOut); 
	    if (list_empty(&tunnel->tunnelHasPendingOut))
		{
		    list_add_tail(&tunnel->tunnelHasPendingOut, &tunnel->netInterface->tunnelHasPendingOut); 
		}
	}
}

//______________________________________________________________________________
// send a packet out if room in the ethernet pseudo-device buffer and room
// in the reliability window.  Returns true if successful, false otherwise.
//______________________________________________________________________________
static
bool
_tunnelSend(Packet *packet, PacketData packetData)
{
    ASSERT(packet);
    Tunnel             *tunnel = packet->tunnel;
    ASSERT(tunnel);
    NetInterface *ni = tunnel->netInterface;
    ASSERT(ni);
    bool anything = false;

    if (netInterfaceSendFull(ni))
	{
	    netInterfaceBlocked();
	    return anything; // cannot send
	}


    if ((tunnel->next - tunnel->start) >= ReliabilityWindowSize)
	{ // no room in reliability window
	    netInterfaceBlocked();
	    return anything; 
	}

    packetPrint("tunnelSend", packet);
    if (PacketNotEmpty == packetData)
	{   // not a pure acknowledge (i.e., has data) so put in reliability window
	    ASSERT(filledBytes(packet) > 2 * sizeof (uint32_t));
	    packetPrint("tunnelSend: put on rw ", packet);

	    uint32 index = tunnel->next % ReliabilityWindowSize;
	    debugXPrint(tunnelDebug, "tunnel->next=$[uint]    index=$[uint]\n", tunnel->next, index);
	    ReliabilityWindow *rw  = &tunnel->reliabilityWindow[index];
    
	    rw->packet             = packet;
	    rw->transmitCount      = 1;

	    // Set the timer
	    reliabilityWindowAdd(rw, &tunnel->rtt);

	    tunnel->next++;
	}

    // if packet is part of a list, remove it from the list
    packetRemoveFromList(packet);
    ASSERT(list_empty(&packet->_packetList));

    tunnelSetAcknowledge(packet); // update acknowledgement

    return netInterfaceSend(packet);
}

//______________________________________________________________________________
// check if any packets need to be resent
//______________________________________________________________________________
int
tunnelResend(ListHead *expiredRwSlots)
{
    int anything = 0;

    if (list_empty(expiredRwSlots)) 
	{
	    return anything;
	}
    debugXPrint(tunnelDebug, "timer went off\n");
	    

    ListHead *v, *tmp;
    list_for_each_safe(v, tmp, expiredRwSlots)
	{
	    ReliabilityWindow *rw = reliabilityWindowFromList(v);
	    ASSERT(rw);
	    Packet *packet = rw->packet;
	    ASSERT(packet);
	    Tunnel *tunnel = packet->tunnel;
	    ASSERT(tunnel);
	    NetInterface *ni= tunnel->netInterface;
	    if (netInterfaceSendFull(ni))
		{   // no room
		    debugXPrint(tunnelDebug, "no room for packet\n");
		    netInterfaceBlocked();
		    return anything;
		}

	    // this should only be recalculated when timeout of packet = current timeout
	    if ((1==rw->transmitCount) && (tunnel->rtt.rto <= (rw->retransmitTime - rw->timestamp)))
		{
		    rttTimeout(&tunnel->rtt);       // Exponential backoff 
		    debugXPrint(tunnelDebug, "Now the RTO is $[llint]ns\n", tunnel->rtt.rto);
		}

	    rw->transmitCount++;

	    reliabilityWindowRemove(rw);
	    reliabilityWindowAdd(rw, &tunnel->rtt);	// set new timer
	
	    tunnelSetAcknowledge(packet);
	    packetPrint("tunnelResend: ", packet);
	    netInterfaceSend(packet);
	    anything++;

	    //netInterfacePrintExpired("tunnelIsNewPacket", ni);
	}
    return anything;
}

//______________________________________________________________________________
/// push out pending packets to a device.  If no pending packets, and there are
/// unacknowledge packets, send out empty packet.
//______________________________________________________________________________
int
tunnelSendBacklog(Tunnel *tunnel)
{
    ASSERT(tunnel);

    int anything = 0;
    Packet *packet;
    ListHead *q, *tmp;
    list_for_each_safe(q, tmp, &tunnel->pendingOut)
	{ // external packet list not empty

	    packet = (Packet *) list_entry(q, Packet, _packetList);
	    if (!_tunnelSend(packet, PacketNotEmpty))
		{   // send failed
		    return anything;
		}
	    anything++;
	}

    if (!anything && tunnel->unackedPacket)
	{ // send empty packet
	    packet = tunnelPacketCreateEmpty(tunnel);
	    if (_tunnelSend(packet, PacketEmpty))
		{ 
		    anything++;
		    tunnel->unackedPacket = 0;
		}
	    // empty packets not put on reliability window, sent and forgotten
	    packetFree(packet);
	}
    return anything;
}

//______________________________________________________________________________
/// sets the acknowledgement field of packet to latest value.  This is used when
/// resending a packet to ensure packet contains latest acknowledgement issue,
/// and is only called for packets which will be immediately set.
//______________________________________________________________________________
void
tunnelSetAcknowledge(Packet *packet)
{
    ASSERT(packet);
    Tunnel *tunnel = packet->tunnel;
    ASSERT(tunnel);

    packetSetAcknowledge(packet,  packet->tunnel->acknowledge);
    tunnel->unackedPacket = 0; // no need to send a empty packet just to ack, piggybacked here
}
