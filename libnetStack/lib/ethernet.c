//______________________________________________________________________________
/// Ethernet packet
//
// Jan-2009: Andrew Trumbo
// Aug-2009: Jon A. Sowlorth
// Based on netfront.c from MiniOS
//______________________________________________________________________________

#include <rpcKernel.h>
#include <ethos/generic/debug.h>
#include <ethos/dual/core.h>
#include <ethos/generic/ethernet.h>
#include <ethos/generic/packet.h>
#include <ethos/generic/envelope.h>

void
ethernetHeaderPrint(char* buffer)
{
    uint16 *ptr = (uint16*) (buffer + 2*EthernetMacSize);

    debugXPrint(tunnelDebug, "  dst: $[mac]  ", (uchar*) buffer);

    debugXPrint(tunnelDebug, "src: $[mac]  ", (uchar*)buffer+EthernetMacSize);

    debugXPrint(tunnelDebug, "size: $[int]  ", ntohs(*ptr));
}

//______________________________________________________________________________
/// checks whether two Ethernet MAC address are equal
//______________________________________________________________________________
int
ethernetMacMatch(unsigned char dest[], unsigned char from_interface[])
{
    int i;
    for (i=0; i < EthernetMacSize; i++) {
	if (dest[i]!=from_interface[i])
	    {
		return 0;
	    }
    }

    return 1;
}

//______________________________________________________________________________
/// create an ethernet header
//______________________________________________________________________________
void
ethernetPacketHeader(unsigned char *buffer,
		     const EthernetMac dstMac,
		     const EthernetMac srcMac)
{
    memcpy(buffer,   dstMac, 6);
    memcpy(buffer+6, srcMac, 6);
    memzero(buffer+12, 2);
}

//______________________________________________________________________________
/// used to determine larger host, MACs must be different
//______________________________________________________________________________
int
ethernetLarger(const EthernetMac mac0,
	       const EthernetMac mac1)
{
    int i;
    for (i=0; i<6; i++)
	{
	    if (mac0[i] < mac1[i])
		return 0;
	    if (mac0[i] > mac1[i])
		return 1;
	}
    debugXPrint(tunnelDebug, " MACs are equal $[mac]\n", mac0);
    BUG();

    return 0;
}



//______________________________________________________________________________
/// extract ethernet header info from payload
//______________________________________________________________________________
bool
ethernetExtract(EnvelopeBuffer *payload, EnvelopeLocal *el, EnvelopeRemote *er)
{
    ASSERT(payload);
    ASSERT(el);
    ASSERT(er);
    uint16  s;
    char8 *buffer = payload->start;

    memcpy(el->mac,  buffer,                   EthernetMacSize);
    memcpy(er->mac,  buffer+EthernetMacSize,   EthernetMacSize);
    s = ntohs(*(uint16*)(buffer+2*EthernetMacSize));
    
    if (s <= 1500)
	{   // a size field
	    payload->size   = s;
	}
    else if (s==EthernetTypeIp)
	{
	    payload->size  -= EthernetHeaderSize;
	}
    else
	{
	    return false;
	}

    payload->start += EthernetHeaderSize;

    return true;
}

//______________________________________________________________________________
// create an ethernet header encapsulating IP layer
//______________________________________________________________________________
void
ethernetCreateHeader(EnvelopeBuffer *payload,
		     bool ipType,
		     EnvelopeLocal  *el,
		     EnvelopeRemote *er
		     )
{
    // add in padding if necessary
    if (payload->size < EthernetMinPayload)
	{
	    memzero(payload->start + payload->size, EthernetMinPayload - payload->size);
	    payload->size = EthernetMinPayload;
	}

    payload->start -= EthernetHeaderSize;
    char *buffer = payload->start;
    memcpy(buffer,                 (void*) &er->mac, EthernetMacSize);
    memcpy(buffer+EthernetMacSize, (void*) &el->mac, EthernetMacSize);

    // for ethernet, size does not include ethernet headers
    uint16      size = ipType ? EthernetTypeIp : payload->size; //IP Protocol
    uint16 *buffSize = (uint16*)(buffer+2*EthernetMacSize);
    *buffSize = htons(size);

    payload->size += EthernetHeaderSize;
    
    //debugXPrint(tunnelDebug, "ethernetCreateHeader:");
    //ethernetHeaderPrint(buffer);
    //debugXPrint(tunnelDebug, "  payload->size = $[uint]   size = $[uint]\n",
    //     payload->size, size);

}
