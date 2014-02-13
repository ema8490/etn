//______________________________________________________________________________
/// Common network headers
//  Sep-2010: Ameet S. Kotian
//______________________________________________________________________________

#ifndef __NETWORKING_H__
#define __NETWORKING_H__

#include <ethos/dual/ref.h>
#include <ethos/dual/ctype.h>

typedef uchar Nonce;
typedef uchar CipherText;
typedef uchar ClearText;

struct EnvelopeRemote_S;
struct EnvelopeLocal_S;
struct EnvelopeBuffer_S;

// -or- together below Header to describe processing for interface
enum {
    HeaderEthernet    = 1,     // has ethernet headers
    HeaderIpUdp       = 1<<1,  // has IP/UDP headers
    HeaderCrypto      = 1<<2,  // has cryptographic headers (and encrypted payload)
    HeaderReliability = 1<<3,  // has reliability headers
    HeaderConnection  = 1<<4   // has connection headers
};

// this belongs in architecture spec
#define __LITTLE_ENDIAN__

#ifndef _NETINET_IN_H
uint16 htons(uint16 n);
uint16 ntohs(uint16 n);
uint32 htonl(uint32 n);
uint32 ntohl(uint32 n);
#endif

uint16  internetChecksum(uint16 *addr, int len) ;
#endif
