//______________________________________________________________________________
// IP headers
// 
// Sep-2010:  Ameet S. Kotian
//______________________________________________________________________________

#ifndef __IP_H__
#define __IP_H__

#include <ethos/generic/networking.h>

struct IpHeader_S {
#if defined(__LITTLE_ENDIAN__)
    uchar8 ihl:4,
	   version:4;
#elif defined (__BIG_ENDIAN__)
    uchar8 version:4,
  	   ihl:4;
#endif
    uchar8	tos;
    int16       totalLength;
    int16       id;
    int16       frag_off;
    uchar8      ttl;
    uchar8	protocol;
    uint16	check;
    IpAddress   sourceIpAddress;
    IpAddress   destinationIpAddress;
};

typedef struct IpHeader_S IpHeader;

bool
ipExtract (struct EnvelopeBuffer_S *payload,
           struct EnvelopeLocal_S *el,
           struct EnvelopeRemote_S *er);
void
ipCreateHeader (struct EnvelopeBuffer_S *payload,
	        struct EnvelopeLocal_S *el,
	        struct EnvelopeRemote_S *er);

#endif
