//______________________________________________________________________________
// Crypto interface routines to DJB's NaCl code
//
// Sep-2010: Ameet S. Kotian
//______________________________________________________________________________

#ifndef __NET_CRYPTO_H__
#define __NET_CRYPTO_H__

#include <ethos/generic/networking.h>

#define NanosecsPerSec          1000000000ull

typedef struct CryptoHeader {
    uint64   tunnelId;
    uint64   nonce;
} CryptoHeader;

typedef enum {
	NetCryptoKeyPermanent,
	NetCryptoKeyEphemeralOutgoing,
	NetCryptoKeyEphemeralIncoming,
} NetCryptoKeyType;

struct EnvelopeLocal_S;
struct EnvelopeRemote_S;
struct Tunnel_S;

void
cryptoPutRandomBytesToPool (unsigned long long xlen);

void
mixinGetRandomBytes(unsigned char *x, unsigned long long xlen);

bool
cryptoComputeSymmetricKey(struct EnvelopeLocal_S *el,
                          struct EnvelopeRemote_S *er,
                          NetCryptoKeyType);

bool
cryptoCreateEncrypted(bool firstPacket,
                      struct EnvelopeBuffer_S *payload,       ///< holds the encrypted result
                      struct EnvelopeBuffer_S  cleartext,     ///< holds the unencrypted input
                      struct EnvelopeLocal_S  *el,  
                      struct EnvelopeRemote_S *er   
			   );

bool
cryptoDecrypt(struct EnvelopeBuffer_S  payload,
              struct EnvelopeBuffer_S *cleartext,
              struct EnvelopeLocal_S  *el,
              struct EnvelopeRemote_S *er,
              struct Tunnel_S *tunnel);


bool
cryptoExtract(struct EnvelopeBuffer_S *payload,
              struct EnvelopeRemote_S *envelopeRemote);

Status
cryptoAuthenticatorIsValid (PublicEncryptionKey  senderPubKey,
                            PublicEncryptionKey  recipientPubKey,
                            PrivateEncryptionKey recipientPrivKey,
                            Authenticator       *authenticator);

Status
cryptoGenAuthenticator (PrivateEncryptionKey senderPrivKey,
                        PublicEncryptionKey  recipientPubKey,
                        Authenticator       *authenticator);

void cryptoInit(void);

#endif
