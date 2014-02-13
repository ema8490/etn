//______________________________________________________________________________
// Network interface for user space or ethos kernel
//
// Jan-2009: Andrew Trumbo
// Aug-2009: Jon A. Solworth
// Sep-2010: Ameet S. Kotian
//______________________________________________________________________________

#ifndef __NET_INTERFACE_H__
#define __NET_INTERFACE_H__

#include <ethos/generic/hashTable.h>
#include <ethos/generic/list.h>
#include <ethos/generic/networking.h>
#include <ethos/generic/packet.h>
#include <ethos/generic/connection.h>
#include <ethos/generic/ethernet.h>
#include <ethos/generic/envelope.h>

//
//  1. for communicating with shadowdaemon
//  2. general ethernet interface
//  3. debug interface
//
#define MaxInterfaces  3

enum {
    HeadersEthosToShadowdaemon = HeaderEthernet|HeaderIpUdp|HeaderReliability|HeaderConnection,
    HeadersNetworking          = HeaderEthernet|HeaderIpUdp|HeaderCrypto|HeaderReliability|HeaderConnection,
    HeadersShadowdaemonToEthos = HeaderReliability|HeaderConnection,
    HeadersTerminalToEthos     = HeaderCrypto|HeaderReliability|HeaderConnection,
};

typedef struct NetInterface_S NetInterface;

typedef enum {
    XenNetInterfaceType,                        ///< interface from within the ethos kernel
    EtherNetInterfaceType,                      ///< interface for Linix userspace ethernet socket
} NetInterfaceType;

struct NetInterface_S { // the public network interface
    uint               interfaceNumber;          ///< ethernet interface number

    NetInterfaceType   netInterfaceType;         ///< type of net interface

    EtnInterfaceType  *defaultRpcInterface;      ///< ETN RPC interface used when creating on-demand tunnels
                                                 ///< on this interface.  Allows shadowdaemon to specify the 
					         ///< shadowdaemon RPC and the terminal to specify the terminal RPC.
					         ///< Normally, this will be the kernel-to-kernel RPC.

    uint               headers;                  ///< which headers are in use?
    uint               headerSize;
    uint               reserveSize;              ///< reserve size
    uint               otherSize;                ///< reliability and connection headers (12)

    EnvelopeLocal      envelopeLocal;            ///< communication parameters


    ListHead           pendingIn;                ///< incoming packets waiting to be processed

    HashTable         *tunnelIdHashTable;        ///< HT of tunnels associated with an interface, indexed by tunnel ID
    HashTable         *tunnelPublicKeyHashTable; ///< HT of tunnels associated with an interface, indexed by public key
    HashTable         *tunnelHostnameHashTable;  ///< HT of tunnels associated with an interface, indexed by hostname

    ListHead           tunnelHasPendingOut;      ///< list of the tunnels that have packets ready to go out

    ListHead           expiredRwSlots;           ///< list of expired reliability window slots waiting to be sent

    int                fd;                       ///< file descriptor for EtherNetInterface or SocketNetInterface
   
    int                if_index;                 ///< used on a raw internet socket

    bool               locked;                   ///< if true, will not create any new tunnels
};

extern NetInterface     netInterface[MaxInterfaces];
extern struct Tunnel_S *shadowdaemonTunnel;
extern struct Tunnel_S *terminalTunnel;

NetInterface *netInterfaceGet(uint interfaceNumber);
bool netInterfaceValid(uint interfaceNumber);

int netInterfaceIncoming(NetInterface *netInterface);
int netInterfaceDo(NetInterface*);
int netInterfaceDoAll(void);
void netInterfaceReceivePacket(Packet *packet);
void netInterfaceBlocked(void);
int  netInterfacePending(NetInterface*);
void netInterfacePrintExpired(char *where, NetInterface *ni);
int  netInterfaceProcess(NetInterface*);
bool netInterfaceSend(Packet *packet);
int  netInterfaceSendFull(NetInterface *netInterface);
bool netInterfaceUpdateEphemeralKey (void);
NetInterface *netInterfaceInit(NetInterfaceType netInterfaceType,
                               uint header,
                               EnvelopeLocal *el,
                               EtnInterfaceType *defaultRpcInterface);

int  netInterfaceCheckHeaders(unsigned char *buffer, int *length, struct Tunnel_S *tunnel ) ;

#endif /* __NET_INTERFACE_H__ */
