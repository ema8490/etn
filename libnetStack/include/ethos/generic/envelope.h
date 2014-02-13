//______________________________________________________________________________
// Envelope: manages ethernet, IP, UDP, and Crypto layers
// 
// Dec-2010:  Jon A. Solworth
//______________________________________________________________________________

#ifndef __ENVELOPE_H__
#define __ENVELOPE_H__

#include <ethos/generic/networking.h>
#include <ethos/generic/ethernet.h>
#include <ethos/generic/ip.h>
#include <ethos/generic/udp.h>
#include <ethos/generic/netCrypto.h>
#include <ethos/generic/packet.h>

typedef struct EnvelopeBuffer_S {
    char      *start;
    ulong      size;
} EnvelopeBuffer;

typedef struct EnvelopeLocal_S
{
    uchar8           mac[EthernetMacSize];
    IpAddress        ipAddress;
    UdpPort          udpPort;
    uint64           nonce;    // for outgoing messages
    CryptoKey        publicKey[CryptoBoxPublicKeySize];                       // The permanent key; used to accept initial connection
                                                                              // when providing directory service to client that does
    CryptoKey        privateKey[CryptoBoxPrivateKeySize];                     // not know the directory service's ephemeral key.
    CryptoKey        ephemeralPublicKeyForIncoming[CryptoBoxPublicKeySize];   // For incoming, lifetime determined by service record.
    CryptoKey        ephemeralPrivateKeyForIncoming[CryptoBoxPrivateKeySize];
    CryptoKey        ephemeralPublicKeyForOutgoing[CryptoBoxPublicKeySize];   // For outgoing, regenerated for each tunnel.
                                                                              // Only the public key needs to remain (to place in a
                                                                              // tunnel's first packet, so don't store the 
                                                                              // corresponding private key here.
} EnvelopeLocal;

// In priority order, match on
//    1. TunnelId 
//    2. mac
typedef struct EnvelopeRemote_S
{
    String          *hostname; // remote hostname so that we may avoid unnecessary directory
                               // service queries over the network
    uchar8           mac[EthernetMacSize];
    IpAddress        ipAddress;
    UdpPort          udpPort;
    uint64           nonce;    // for incoming messages
    CryptoKey        publicKey[CryptoBoxPublicKeySize];
    CryptoKey        ephemeralPublicKey[CryptoBoxPublicKeySize];

    uint64           cryptoTunnelId;
    CryptoKey        symmetricKey[CryptoSymmetricKeySize];
    bool             oddSide;   // true if odd nonces/connections generated
    bool             publicKeyPresent;
} EnvelopeRemote;

struct NetInterface_S;
struct Tunnel_S;
struct Packet_S;

void envelopeInit(struct NetInterface_S *ni);
Packet *envelopeOpen(struct NetInterface_S *ni, EnvelopeBuffer payload);
Packet* envelopeOpenStartingAtCrypto(struct NetInterface_S *ni, EnvelopeBuffer payload, EnvelopeLocal envelopeLocal, EnvelopeRemote envelopeRemote);
bool envelopeCreate(struct Tunnel_S *tunnel, struct Packet_S *packet, EnvelopeBuffer  *page);
void envelopeLocalPrint(EnvelopeLocal *el);
void envelopeRemotePrint(EnvelopeRemote *er);
struct Tunnel_S* envelopeFindTunnelByPublicKey(struct NetInterface_S *ni, CryptoKey     publicKey[CryptoBoxPublicKeySize]);
struct Tunnel_S* envelopeFindTunnelByHostname(struct NetInterface_S *ni,  String       *hostname);
void   envelopeGetTunnelId(uchar* tunnelId,
			   uint32 headers,
			   EnvelopeLocal  *el,
			   EnvelopeRemote *er);
EnvelopeLocal envelopeLocalCreate(EthernetMac mac, IpAddress ip, UdpPort port);
EnvelopeRemote envelopeRemoteCreate(EthernetMac mac, IpAddress ip, UdpPort port);
#endif
