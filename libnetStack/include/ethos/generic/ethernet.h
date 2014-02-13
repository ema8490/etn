#ifndef __ETHERNET_H__
#define __ETHERNET_H__

#include <ethos/generic/networking.h>

struct EnvelopeLocal_S;
struct EnvelopeRemote_S;

void
ethernetCreateHeader(struct EnvelopeBuffer_S *paylaod,
		     bool ipType,
		     struct EnvelopeLocal_S  *el,
		     struct EnvelopeRemote_S *er
		     );

int  ethernetMacMatch(EthernetMac dest, EthernetMac from_interface);

int  ethernetLarger(const EthernetMac, const EthernetMac);

void ethernetHeaderPrint(char* buffer);

bool ethernetExtract(struct EnvelopeBuffer_S *payload,
                     struct EnvelopeLocal_S *el,
                     struct EnvelopeRemote_S *er);

void ethernetPacketHeader(unsigned char *buffer,
			  const unsigned char *dstMac,
			  const unsigned char *srcMac);

#endif
