//______________________________________________________________________________
// Tunnel is between two network interfaces and carries multiple connections
//
// Sep-2010: Jon A. Solworth
//______________________________________________________________________________

#ifndef __TUNNEL_H__
#define __TUNNEL_H__

#include <ethos/generic/networking.h>
#include <ethos/generic/packet.h>
#include <ethos/generic/ethernet.h>
#include <ethos/generic/ip.h>
#include <ethos/generic/udp.h>
#include <ethos/generic/netCrypto.h>
#include <ethos/generic/rtt.h>
#include <ethos/generic/reliabilityWindow.h>
#include <ethos/generic/envelope.h>

typedef struct Tunnel_S Tunnel;

typedef struct ConnectionInfo_S ConnectionInfo;

struct NetInterface_S;


enum { ReliabilityWindowSize = 128, HalfReliabilityWindowSize=8};

struct Tunnel_S {
    struct
    NetInterface_S    *netInterface;           ///< what is blocked on

    EtnInterfaceType  *rpcInterface;           ///< RPC interface associated with tunnel

    uint               maximumPayloadSize;     ///< largest payload excluding ethernet headers and packetReserve
                                               ///< (in bytes)


                                               ///  connection management
    struct Connection_S 
                     **connectionArray;        ///< associated connections
    long               connectionAllocated;    ///< number of connections allocated
    long               connectionFree;         ///< first free connection


    struct Connection_S
                      *controlConnection;      /// the control connection

                                               /// reliability     
    ReliabilityWindow
                       reliabilityWindow[ReliabilityWindowSize];
    Packet            *incoming[ReliabilityWindowSize];

    uint32             start;                  ///< sequence of the first element in ReliabilityWindow

    uint32             next;                   ///< sequence of one past the last element in the ReliabilityWindow

    uint32             sequence;               ///< next sequence number to be sent

    uint32             acknowledge;            ///< highest sequence number received

    bool               unackedPacket;          ///< true if packet received but not acknowledge
                                               ///< whenever a packet (or sequence of packets)
                                               ///< is received we acknowledge it

    ListHead           pendingOut;             ///< packets which are yet to be sent
    ListHead           tunnelHasPendingOut;    ///< list of the tunnels that have packets ready to go out
                                               ///< this packet serves only as a list heads for to-be-sent packets
    EnvelopeRemote     envelopeRemote;         ///< remote envelope information

    Rtt                rtt;                    ///< Information about round trip timeout 

    ListHead tunnelIdList;                     ///< list of tunnels associated with an interface, used with netInterface hash table
    ListHead tunnelPublicKeyList;              ///< list of tunnels associated with an interface, used with netInterface hash table
    ListHead tunnelHostnameList;               ///< list of tunnels associated with an interface, used with netInterface hash table

    uchar              tunnelId[TunnelIdSize]; ///< tunnel Id
};



Tunnel *tunnelCreate(struct NetInterface_S *netInterface,
                     EnvelopeRemote *er,
                     EtnInterfaceType *rpcInterface,
                     bool isOutgoing);

bool    tunnelConnectionIdValid(Tunnel *tunnel, uint32 connectionId);
int     tunnelResend(ListHead *expiredRwSlots);
void    tunnelOut(Packet *packet);
int     tunnelSendBacklog(Tunnel *tunnel);
bool    tunnelIsNewPacket(Packet *packet);
Packet *tunnelIsWaiting(Packet *packet);
Packet* tunnelPacketCreate(Tunnel *tunnel, uint32 size);
Packet* tunnelPacketCreateFirst(Tunnel *tunnel, uint32 size);
Packet* tunnelPacketCreateMax(Tunnel *tunnel);
void    tunnelFree(Tunnel *);
uint32  tunnelConnectionAllocate(Tunnel *tunnel);
void    tunnelProcessInPacket(Packet *packet);
int     tunnelPacketConnections(Packet *packet);
void    tunnelSetAcknowledge(Packet *packet);
void    tunnelInit(void);
#endif
