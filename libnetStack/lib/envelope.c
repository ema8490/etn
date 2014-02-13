//______________________________________________________________________________
/// envelope: the envelope headers are
//      1. Ethernet
//      2. IP
//      3. UDP
//      3. Crypto (optional)
//  Opening the envelope produces a packet with reliability and connection layers
//
//  Dec-2010: Jon A. Solworth
//______________________________________________________________________________

#include <rpcKernel.h>
#include <ethos/generic/debug.h>
#include <ethos/generic/hashTable.h>
#include <ethos/dual/core.h>
#include <ethos/generic/networking.h>
#include <ethos/generic/netInterface.h>
#include <ethos/generic/tunnel.h>
#include <ethos/generic/envelope.h>
#include <ethos/generic/connection.h>

//______________________________________________________________________________
// Create the network header table to calculate the header offsets, header sizes.
//______________________________________________________________________________
void 
envelopeInit(NetInterface *ni)
{
    ASSERT (NULL != ni);

    ni->reserveSize = 0;
    ni->headerSize = 0;
    ni->otherSize = 0;

    if (ni->headers & HeaderEthernet)
	{
	    ni->headerSize += EthernetHeaderSize;
	}

    if (ni->headers & HeaderIpUdp)
	{
	    ni->headerSize += IpUdpHeaderSize;
	}

    if (ni->headers & HeaderCrypto)
	{
	    ni->headerSize += CryptoHeaderSize;
	    ni->reserveSize = CryptoReserveSize;
	}

    if (ni->headers & HeaderReliability)
	{
	    ni->otherSize += ReliabilityHeaderSize;
	}

    if (ni->headers & HeaderConnection)
	{
	    ni->otherSize += ConnectionHeaderSize;
	}

    debugXPrint(envelopeDebug, "headerSize = $[uint]   reserveSize = $[uint]   otherSize = $[uint]\n",
                ni->headerSize, ni->reserveSize, ni->otherSize);
}
    
//______________________________________________________________________________
/// Create a new envelope, with Ethernet, IP, UDP, and crypto headers as required;
/// return the envelope as page.
//______________________________________________________________________________
bool
envelopeCreate(Tunnel         *tunnel,
               Packet         *packet,
               EnvelopeBuffer *page)
{
    ASSERT(NULL != tunnel);
    ASSERT(NULL != packet);

    bool             fnval = true;
    NetInterface       *ni = tunnel->netInterface;
    bool       firstPacket = packet->firstPacket;
    uint32    computedSize = filledBytes(packet);
    EnvelopeBuffer payload = { page->start + ni->headerSize, computedSize };

    // Do the crypto first.
    if (ni->headers & HeaderCrypto)
	{
	    EnvelopeBuffer cleartext = { (char*) packet->_start, computedSize };
	    if (! cryptoCreateEncrypted(firstPacket, &payload, cleartext, &ni->envelopeLocal, &tunnel->envelopeRemote))
		{
		    fnval = false;
		    goto done;
		}
	    computedSize += CryptoReserveSize + CryptoHeaderSize + firstPacket * CryptoBoxPublicKeySize;
	    ASSERT(computedSize == payload.size);
	} 
    else
	{
	    memcpy(payload.start, packet->_start, computedSize);
	}

    if (ni->headers & HeaderIpUdp)
	{
	    // Update IP and UDP with checksums.
	    udpCreateHeader(&payload, &ni->envelopeLocal, &tunnel->envelopeRemote);
	    ipCreateHeader (&payload, &ni->envelopeLocal, &tunnel->envelopeRemote);
	    computedSize += IpUdpHeaderSize;
	    ASSERT(computedSize == payload.size);
	}

    if (ni->headers & HeaderEthernet)
	{
	    ethernetCreateHeader(&payload, ni->headers & HeaderIpUdp, &ni->envelopeLocal, &tunnel->envelopeRemote);
	    computedSize += EthernetHeaderSize;
	    ASSERT((computedSize == payload.size)
	       || ((computedSize < payload.size) && (payload.size == EthernetMinPayload + EthernetHeaderSize)));
	}

    page->size = payload.size;

    ASSERT(payload.start == (char*) page->start);

done:
    return fnval;
}

//______________________________________________________________________________
// get the tunnelId from er and el
//______________________________________________________________________________
void
envelopeGetTunnelId(uchar* tunnelId,
		    uint32 headers,
		    EnvelopeLocal  *el,
		    EnvelopeRemote *er)
{
    memzero((void*)tunnelId, TunnelIdSize);

    if (headers & HeaderCrypto)
	{
	    memcpy((void*)tunnelId, (void*) &er->cryptoTunnelId, sizeof(uint64));
	}
    else if (headers & HeaderEthernet)
	{
	    memcpy(tunnelId,                 el->mac, EthernetMacSize);
	    memcpy(tunnelId+EthernetMacSize, er->mac, EthernetMacSize);
	}
}


//______________________________________________________________________________
/// Create a new tunnel for ni, in response to an arriving packet.
/// Requires that there does not already exist a tunnel at that address.
//______________________________________________________________________________
static
Tunnel*
_envelopeCreateTunnel(NetInterface   *ni,
                     EnvelopeRemote *er)
{
    envelopeRemotePrint(er);

    ASSERT(NULL != ni);
    ASSERT(NULL != er);

    Tunnel *tunnel = tunnelCreate(ni, er, ni->defaultRpcInterface, false);

    return tunnel;
}

//______________________________________________________________________________
/// Finds the tunnel at a given remote address within ni.
//______________________________________________________________________________
static
Tunnel*
_envelopeFindTunnel(NetInterface   *ni,
                   EnvelopeRemote *er)
{
    ASSERT(NULL != ni);
    ASSERT(NULL != ni->tunnelIdHashTable);
    ASSERT(NULL != er);

    uchar tunnelId[TunnelIdSize];
    envelopeGetTunnelId(tunnelId, ni->headers, &ni->envelopeLocal, er);
    ListHead *l = hashTableFind (ni->tunnelIdHashTable, tunnelId);

    return l ? list_entry (l, Tunnel, tunnelIdList) : (Tunnel *) NULL;
}

//______________________________________________________________________________
/// Finds the tunnel produced using the given public key.
//______________________________________________________________________________
Tunnel*
envelopeFindTunnelByPublicKey(NetInterface *ni,
                              CryptoKey     publicKey[CryptoBoxPublicKeySize])
{
    ASSERT(NULL != ni);
    ASSERT(NULL != ni->tunnelPublicKeyHashTable);

    ListHead *l = hashTableFind (ni->tunnelPublicKeyHashTable, publicKey);

    return l ? list_entry (l, Tunnel, tunnelPublicKeyList) : (Tunnel *) NULL;
}

//______________________________________________________________________________
/// Finds the tunnel with the given public key.
//______________________________________________________________________________
Tunnel*
envelopeFindTunnelByHostname(NetInterface *ni,
                             String       *hostname)
{
    ASSERT(NULL != ni);
    ASSERT(NULL != ni->tunnelHostnameHashTable);
    ASSERT(NULL != hostname && refSize(hostname) <= MaxHostnameSize); // RFC 1034 + terminating NULL.

    ListHead *l = hashTableFind (ni->tunnelHostnameHashTable, (HashKey) hostname);

    return l ? list_entry (l, Tunnel, tunnelHostnameList) : (Tunnel *) NULL;
}

//______________________________________________________________________________
/// Create a new EnvelopeLocal with the given mac, ip, and port.
//______________________________________________________________________________
EnvelopeLocal
envelopeLocalCreate(EthernetMac mac,
                    IpAddress ip,
                    UdpPort port)
{
    EnvelopeLocal el;

    memzero((void*) &el, sizeof(el));
	   
    el.ipAddress = ip;
    el.udpPort   = port;
    memcpy(el.mac, mac, EthernetMacSize);

    return el;
}

//______________________________________________________________________________
/// Create a new EnvelopeRemote with the given mac, ip, and port.
//______________________________________________________________________________
EnvelopeRemote
envelopeRemoteCreate(EthernetMac mac,
		     IpAddress ip,
		     UdpPort port)
{
    EnvelopeRemote er;

    memzero((void*)&er, sizeof(er));

    er.ipAddress = ip;
    er.udpPort   = port;
    memcpy(er.mac, mac, EthernetMacSize);

    return er;
}

//______________________________________________________________________________
// Open the envelope, starting at the crypto layer.
//    If successful, the envelope is processed
//    otherwise, the envelope is discarded.
//    The purpose of this entry point is to allow
//    integration with our POSIX network-based library
//    (in this case, the IP address and UDP port are
//    known only to POSIX, and the HeaderIpUdp flag is 
//    not set so this code never receives UDP/IP headers.)
//______________________________________________________________________________
Packet*
envelopeOpenStartingAtCrypto(NetInterface *ni,
                             EnvelopeBuffer payload,
                             EnvelopeLocal envelopeLocal,
                             EnvelopeRemote envelopeRemote)
{
    ASSERT(NULL != ni);

    Packet *packet = NULL;

    if  (ni->headers & HeaderCrypto)
	{
	    if (! cryptoExtract(&payload, &envelopeRemote))
		{
		    goto done;
		}
	}

    Tunnel *tunnel = _envelopeFindTunnel(ni, &envelopeRemote);
    if (tunnel)
	{
	    if (envelopeRemote.publicKeyPresent)
		{
		    if (0!=memcmp(envelopeRemote.ephemeralPublicKey, tunnel->envelopeRemote.ephemeralPublicKey, CryptoBoxPublicKeySize))
			{
			    debugXPrint(envelopeDebug,
				     "oops, public key present on existing tunnel "
				     "does not match tunnel's key\n");
                            debugXPrint(tunnelDebug, "packet key = $[pubboxkey]", envelopeRemote.ephemeralPublicKey);
			    debugXPrint(tunnelDebug, "tunnel key = $[pubboxkey]", tunnel->envelopeRemote.ephemeralPublicKey);

			    goto done; // A bad packet.
			}
		}
	}
    else if (! ni->locked)
	{   // Create a new tunnel.
	    if  ((ni->headers & HeaderCrypto) && (!envelopeRemote.publicKeyPresent))
		{
		    debugXPrint(envelopeDebug, "oops, public key not present on created tunnel\n");
		    envelopeRemotePrint(&envelopeRemote);
		    goto done; // Not really a bug, just a bad packet.
		}
	    tunnel = _envelopeCreateTunnel(ni, &envelopeRemote);
	    if (NULL == tunnel)
		{
		    goto done;
		}
	    debugXPrint(envelopeDebug, "New tunnel created.\n");
	    envelopeLocalPrint(&envelopeLocal);
	    envelopeRemotePrint(&envelopeRemote);
	}
    else
	{
	    goto done;
	}


    static char buffer[2048];
    EnvelopeBuffer cleartext = {buffer, sizeof (buffer)};

    if  (ni->headers & HeaderCrypto)
	{
	    if (envelopeRemote.nonce <= tunnel->envelopeRemote.nonce)
		{
		    debugXPrint(envelopeDebug,
			     "bad nonce, ignored --  nonce = $[lluint]  el nonce = $[lluint]  tunnel nonce = $[lluint]\n",
			     envelopeRemote.nonce, envelopeLocal.nonce, tunnel->envelopeRemote.nonce);
		    goto done;
		}
	    
	    if (! cryptoDecrypt(payload, &cleartext, &envelopeLocal, &envelopeRemote, tunnel))
		{
		    if (! cryptoComputeSymmetricKey(&ni->envelopeLocal, &tunnel->envelopeRemote, NetCryptoKeyPermanent))
			{
			    goto done;
			}

		    if (! cryptoDecrypt(payload, &cleartext, &envelopeLocal, &envelopeRemote, tunnel))
			{
			    goto done;
			}
		}
	    debugXPrint(envelopeDebug, "Packet decrypted.   nonce=$[lluint]\n", envelopeRemote.nonce);
	    tunnel->envelopeRemote.nonce = envelopeRemote.nonce;
	}
    else
	{
	    cleartext = payload;
	}

    ulong length = cleartext.size;
    packet = packetAllocate(length);
    packet->tunnel = tunnel;
    packetAppend(packet, (uint8_t *) cleartext.start, length);
    resetCurrent(packet);
    
    debugXPrint(envelopeDebug, "Before tunnelIsNewPacket.\n");

    // now process the packet for the reliability and connection layers
    if (tunnelIsNewPacket(packet))
	{
	    debugXPrint(envelopeDebug, "valid packet found!\n");
	    envelopeRemotePrint(&envelopeRemote);
	    goto done;
	}

    // Not useful; toss.
    packetFree(packet); 
    packet = NULL;

done:
    return packet;
}

//______________________________________________________________________________
// Open the envelope.
//    If successful, the envelope is processed
//    otherwise, the envelope is discarded.
//______________________________________________________________________________
Packet*
envelopeOpen(NetInterface *ni,
	     EnvelopeBuffer payload)
{
    ASSERT(NULL != ni);

    Packet          *fnval = NULL;
    EnvelopeLocal    envelopeLocal;
    EnvelopeRemote   envelopeRemote;
    memzero(&envelopeLocal, sizeof(envelopeLocal));
    memzero(&envelopeRemote, sizeof(envelopeRemote));

    ulong  length = payload.size;
    if (length < ni->headerSize)
	{
	    debugXPrint(envelopeDebug, "received packet too small ($[int] < $[int])\n", length, ni->headerSize);
	}

    if (ni->headers & HeaderEthernet)
	{
            //debugXPrint(envelopeDebug, "extracting ethernet  payload.size = $[uint]\n", payload.size);
	    if (! ethernetExtract(&payload, &envelopeLocal, &envelopeRemote))
		{
		    goto done;
		}
	}

    if (ni->headers & HeaderIpUdp)
	{
	    if (! ipExtract(&payload, &envelopeLocal,   &envelopeRemote))
		{
		    goto done;
		}

	    if (! udpExtract(&payload, &envelopeLocal, &envelopeRemote))
		{
		    goto done;
		}

	    // The shadowdaemon interface is not encrypted. This means that we can
	    // receive all kinds of "bogus" packets. It seems that mDNS/SD on Dom0 will
	    // send such packets to Ethos. These packets will cause a failure
	    // later because they almost definately have a bad connection ID (it seems
	    // to be a race to see who will send the first packet---shadowdaemon or
	    // mDNS/SD. So, at least check that UDP port is the expected shadowdaemon port
	    // here. Check IP address too.
	    if (shadowdaemonTunnel
		&& envelopeRemote.ipAddress == shadowdaemonTunnel->envelopeRemote.ipAddress
		&& envelopeRemote.udpPort == shadowdaemonTunnel->envelopeRemote.udpPort)
		{
		    debugXPrint(envelopeDebug, "packet from shadowdaemon's interface; port ($[int])\n", envelopeRemote.udpPort);
		}
	    else if (terminalTunnel 
		     && envelopeRemote.ipAddress == terminalTunnel->envelopeRemote.ipAddress
		     && envelopeRemote.udpPort   == terminalTunnel->envelopeRemote.udpPort)
		{
		    debugXPrint(envelopeDebug, "packet from terminal's interface; port ($[int])\n", envelopeRemote.udpPort);
		}
	    else if (! (ni->headers & HeaderCrypto))
		{
		    debugXPrint(envelopeDebug,
		               "ignoring packet on shadowdaemon's/terminal's interface; wrong port ($[int])\n", envelopeRemote.udpPort);
		    goto done;
		}
	}
    
    fnval = envelopeOpenStartingAtCrypto(ni, payload, envelopeLocal, envelopeRemote);

done:
    return fnval;
}


//______________________________________________________________________________
/// Print a local envelope.
//______________________________________________________________________________
void
envelopeLocalPrint(EnvelopeLocal *el)
{
    debugXPrint(envelopeDebug, "    mac =  $[mac]   ip = $[ip]    udp = $[uint]    nonce = $[lluint]\n",
	     el->mac,
	     el->ipAddress,
	     el->udpPort,
	     el->nonce);
}

//______________________________________________________________________________
/// Print a remote envelope.
//______________________________________________________________________________
void
envelopeRemotePrint(EnvelopeRemote *er)
{
    debugXPrint(envelopeDebug, "    mac = $[mac]   ip = $[ip]    udp = $[uint]    nonce = $[lluint]   (crypto)tunnelId = $[lluint]\n",
	     er->mac,
	     er->ipAddress,
	     er->udpPort,
	     er->nonce,
	     er->cryptoTunnelId);
}
