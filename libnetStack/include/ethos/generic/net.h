//______________________________________________________________________________
// Network-related functions useful in kernel- and userspace
//______________________________________________________________________________

#ifndef _ETHOS_NET_H_
#define _ETHOS_NET_H_

#ifdef _ETHOS_KERNEL
#include <ethos/dual/core.h>
#endif

char      toHex(int);
bool      ethernetMacToString(EthernetMacString str, EthernetMac src);
bool      ethernetMacFromString(EthernetMac, const EthernetMacString);
bool      ipAddressToString(IpAddressString str, IpAddress src);
IpAddress ipAddressFromString(IpAddressString str);

#endif
