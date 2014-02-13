//______________________________________________________________________________
// Udp packet
//
// May-2010: Ameet S Kotian
//______________________________________________________________________________

#include <rpcKernel.h>
#include <ethos/dual/core.h>
#include <ethos/generic/udp.h>
#include <ethos/generic/envelope.h>

//______________________________________________________________________________
// note that the checksum is optional, so it is ignored
//______________________________________________________________________________
bool
udpExtract(EnvelopeBuffer *payload,
	   EnvelopeLocal *el,
	   EnvelopeRemote *er
	   )
{
    char *buffer = payload->start;
    UdpHeader *header = (UdpHeader *) buffer;
    er->udpPort = ntohs(header->source);
    el->udpPort = ntohs(header->destination);
    if (payload->size != ntohs(header->length))
	{
	    return false;
	}
    payload->start += UdpHeaderSize;
    payload->size  -= UdpHeaderSize;
    return true;
}

//_____________________________________________________________________________
//  Fill the Udp Header.
//______________________________________________________________________________
void 
udpCreateHeader(EnvelopeBuffer *payload,  // Buffer for the packet headers including
		                          // ethernet, ip, udp and reliability.
		EnvelopeLocal *el,
		EnvelopeRemote *er
		)
{
    ASSERT(payload);
    ASSERT(el);
    ASSERT(er);

    payload->start -= UdpHeaderSize;
    payload->size  += UdpHeaderSize;

    UdpHeader     *udp = (UdpHeader *)(payload->start);
    udp->source        = htons(el->udpPort);
    udp->destination   = htons(er->udpPort);
    udp->checksum      = 0;                    // Must be set once the IP header is filled and
                                               // the length of the data is known.
    udp->length        = htons(payload->size); // Length includes the header.
}
