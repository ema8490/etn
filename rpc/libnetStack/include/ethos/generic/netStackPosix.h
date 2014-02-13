//______________________________________________________________________________
// Network interface for user space or ethos kernel
//
// Sep-2010: W. Michael Petulo
//______________________________________________________________________________

#ifndef __NET_STACK_POSIX_H__
#define __NET_STACK_POSIX_H__

// XXX: This is implemented in dom0/shadowdaemon, but also used in
// dom0/librpc. Should the implementation be moved somewhere else?
//

#include <ethos/generic/packet.h>
#include <ethos/generic/envelope.h>
#include <ethos/generic/ethernet.h>
#include <ethos/generic/netInterface.h>
#include <ethos/generic/envelope.h>

NetInterface *
netInterfaceBind (uint headers,
                  char *networkIface,
                  const IpAddress localIp,
                  const uint16 localPort,
                  const CryptoKey *publicKey,
                  const CryptoKey *privateKey,
                  EtnInterfaceType *rpcInterface);

// The "external" interface on our two-interface ARP proxy router.
// The Dom0-side virtual interface vifX.Y will be given the same
// IP address and this is what we need. Since vifX.Y might not
// exist yet when shadowdaemon runs, we cheat and use eth0.
extern const char *localEthernetName;

// A high-level function to set up an Ethos network interface.
NetInterface *
netInterfaceSetup (bool doSetEthosArpEntries);

// A high-level function to set up an Ethos network tunnel.
Tunnel *
netInterfaceTunnel (NetInterface *interface,
const IpAddress remoteIp,
const uint16 remotePort,
const CryptoKey *publicKey,
EtnInterfaceType *rpcInterface);

IpAddress netInterfaceGetLocalIpAddress(const char name[]);

int netInterfaceGetRemoteMacAddress(unsigned int ifnum, uchar *result);

IpAddress netInterfaceGetRemoteIpAddress(unsigned int ifNum);

void netInterfaceAddArpTableEntry(IpAddress ipAddr, uchar8 *ethAddr);

#endif /* __NET_STACK_POSIX_H__ */
