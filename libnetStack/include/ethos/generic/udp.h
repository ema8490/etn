//______________________________________________________________________________
//  UDP networking header
//  Sep-2010:  Ameet S. Kotian
//______________________________________________________________________________
#ifndef __UDP_H__
#define __UDP_H__

#include <ethos/generic/networking.h>
#include <ethos/generic/ethernet.h>
#include <ethos/generic/ip.h>

struct UdpHeader_S {
    UdpPort  source;
    UdpPort  destination;
    uint16   length;
    uint16   checksum;
} ;

typedef struct UdpHeader_S UdpHeader;



// this is used for computing UDP checksums
struct PseudoHeader_S {
    IpAddress           sourceIp;
    IpAddress           destIp;
    uchar8              reserved;
    uchar8              protocol;
    uint16              udpLength;
};

typedef struct PseudoHeader_S PseudoHeader  ;

void *udpInit(UdpPort source, UdpPort dest, int isPeerSet, int offset);

bool
udpExtract(struct EnvelopeBuffer_S *payload,
	   struct EnvelopeLocal_S *el,
	   struct EnvelopeRemote_S *er
	   );

void
udpCreateHeader(struct EnvelopeBuffer_S *payload,
		struct EnvelopeLocal_S *el,
		struct EnvelopeRemote_S *er
	   );

#endif
