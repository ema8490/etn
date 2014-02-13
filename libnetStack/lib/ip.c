//______________________________________________________________________________
// IPv4 packet
//
// May-2010: Ameet S Kotian
//______________________________________________________________________________

#include <rpcKernel.h>
#include <ethos/generic/debug.h>
#include <ethos/generic/ip.h>
#include <ethos/generic/envelope.h>

//_____________________________________________________________________________
// Fill the IP headers.
//______________________________________________________________________________
void 
ipCreateHeader(EnvelopeBuffer *payload,
	       EnvelopeLocal  *el,
	       EnvelopeRemote *er
	       )
{
    ASSERT (payload);
    ASSERT (el);
    ASSERT (er);

    payload->start -= IpHeaderSize;
    payload->size  += IpHeaderSize;

    IpHeader *ip = (IpHeader *) payload->start;
    ASSERT (ip);

    ip->version  = 4;
    ip->ihl      = (IpHeaderSize) / 4 ;
    ip->tos      = 0;
    ip->id       = 0;  // Can be set later
    ip->frag_off = 0;
    ip->protocol = 17; // User Datagram Protocol
    ip->ttl      = 64; // The current standards recommended default time to live (TTL)
                       // for the Internet Protocol (IP) [45,105] is 64. [RFC 1122]
    ip->totalLength          = htons (payload->size);
    ip->sourceIpAddress      = htonl (el->ipAddress);
    ip->destinationIpAddress = htonl (er->ipAddress);

    ip->check    = 0;  // Set to 0 for the purpose of calculating.
    ip->check    = internetChecksum ((uint16_t *) ip, ip->ihl * 4);
}

//_____________________________________________________________________________
/// Check that the incoming packet has well-formed IP headers.
//______________________________________________________________________________
bool
ipExtract(EnvelopeBuffer *payload,
	  EnvelopeLocal *el,
	  EnvelopeRemote *er)
{
    ASSERT(el);
    ASSERT(er);

    IpHeader *ipHeader = (IpHeader*) payload->start; 

    if (4 != ipHeader->version)
	{ // Oops, not IPv4.
	    return false;
	}

    if (17 != ipHeader->protocol)
	{ // Oops, not UDP.
	    return false;
	}

    el->ipAddress = ntohl(ipHeader->destinationIpAddress);
    er->ipAddress = ntohl(ipHeader->sourceIpAddress);
    // length includes headers
    uint32 totalLength = ntohs(ipHeader->totalLength);
    if (payload->size != totalLength)
	{
	    if (payload->size != EthernetMinPayload)
		{ // Ethernet minimum payload size is 60 - 14 = 46, but we use a number divisible by 4.
		    return false;
		}
	}

    payload->size   = totalLength - IpHeaderSize; // Get rid of Ethernet padding, if any.
    payload->start += IpHeaderSize;

    return true;
}
