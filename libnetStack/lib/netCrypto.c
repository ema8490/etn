//_____________________________________________________________________________
// Crypto functions for crypto headers.
//
// External functions:
//    cryptoInit
//    cryptoComputeSymmetricKey
//    cryptoCreate
//    cryptoExtract
//    cryptoDecrypt
//
// Jun 2010: Ameet S Kotian
// Dec 2010: Jon A. Solworth
//
// Uses Prof. Daniel Bernstein's NACL crypto library.
//______________________________________________________________________________

#include <rpcKernel.h>
#include <ethos/dual/status.h>
#include <ethos/dual/core.h>
#include <ethos/generic/debug.h>
#include <ethos/generic/networking.h>
#include <ethos/generic/netCrypto.h>
#include <ethos/generic/crypto.h>
#include <ethos/generic/envelope.h>
#include <ethos/generic/tunnel.h>
#include <ethos/generic/netInterface.h>

static const int    NonceWindowSecs    = 240;
static const uint64 CryptoTunnelIdMask = ((uint64)1)<<63;

static
inline
char *
_netRead64(char *buff, uint64 *v) 
{
    ASSERT (NULL != buff);
    ASSERT (NULL != v);

    uint32 high;
    uint32 low;
    memcpy((void*)&high, buff, sizeof(high));
    buff += sizeof(high);
    memcpy((void*)&low,  buff, sizeof(low));
    buff += sizeof(low);
    *v = (((uint64) high) << 32) + low;

    return buff;
}

static
inline
char *
_netReadPk(char *buff, uchar *publicKey) 
{
    ASSERT (NULL != buff);
    ASSERT (NULL != publicKey);

    memcpy(publicKey, buff, CryptoBoxPublicKeySize);
    buff += CryptoBoxPublicKeySize;

    return buff;
}

static
inline
char *
_netWrite64(char *buff, uint64 v) 
{
    ASSERT (NULL != buff);

    uint32 high = v >>32;
    uint32 low  = v;
    memcpy(buff, (void*)&high, sizeof(high));
    buff += sizeof(high);
    memcpy(buff, (void*)&low,  sizeof(low));
    buff += sizeof(low);

    return buff;
}

static
inline
char *
_netWritePk(char *buff, uchar *publicKey) 
{
    ASSERT (NULL != buff);
    ASSERT (NULL != publicKey);

    memcpy(buff, publicKey, CryptoBoxPublicKeySize);
    buff += CryptoBoxPublicKeySize;
    return buff;
}

//______________________________________________________________________________
/// Check if this is the first packet from host (a tunnel-establishing packet
/// with a public key present).
//______________________________________________________________________________
static
bool
_cryptoTunnelIdIsFirst(uint64 cryptoTunnelId)
{
    return cryptoTunnelId & CryptoTunnelIdMask;
}

//______________________________________________________________________________
/// Clear the first packet flag.
//______________________________________________________________________________
static
uint64
_cryptoTunnelIdClearFirst(uint64 *cryptoTunnelId)
{
    ASSERT (NULL != cryptoTunnelId);

    *cryptoTunnelId &= ~CryptoTunnelIdMask;

    return *cryptoTunnelId;
}

//______________________________________________________________________________
/// Set the first packet flag.
//______________________________________________________________________________
static
uint64
_cryptoTunnelIdSetFirst(uint64 *cryptoTunnelId)
{
    ASSERT (NULL != cryptoTunnelId);

    *cryptoTunnelId |= CryptoTunnelIdMask;

    return *cryptoTunnelId;
}

//_____________________________________________________________________________
/// Write the nonce in a form which NaCL expects
//_____________________________________________________________________________
static
void
_cryptoNonceWrite(uint64 nonce64,
                 Nonce *nonce)
{
    ASSERT (NULL != nonce);

    // First the 64-bit nonce value.
    nonce = (Nonce *) _netWrite64((char *) nonce, nonce64);
    // Next zero fill.
    memzero(nonce, crypto_box_NONCEBYTES - sizeof(uint64));
}

//_____________________________________________________________________________
// Generate a nonce.
// The nonce is either odd/even depending on oddSide.
// The nonce is monotonically increasing in each direction.
//_____________________________________________________________________________
static
void 
_cryptoNonceGenerate(EnvelopeLocal  *el,
		    EnvelopeRemote *er)
{
    ASSERT(NULL != el);
    ASSERT(NULL != er);

    uint64 nanosecs;
    // Time timeNow =  getTimeOfDay() ;
    // nanosecs = (uint64)(timeNow.seconds * NanosecsPerSec + timeNow.nanoseconds);
    
    // Time not used to generate nonces since clock skew error on virtual machine
    // since time go backwards sometimes.
    // Instead we use an incrementing counter.
    nanosecs = el->nonce;
    if (er->oddSide)
	{
	    nanosecs |= 0x1;
	}
    else 
	{
	    nanosecs &= (~0x1);
	}

    if (nanosecs <= el->nonce)
	{
	    nanosecs = el->nonce + 2;
	}

    ASSERT(nanosecs > el->nonce);

    el->nonce = nanosecs;
}

//_____________________________________________________________________________
// Generate a tunnel ID.
// The tunnel ID is the encrypted time of day.
//_____________________________________________________________________________
static
uint64
_cryptoTunnelIdGenerate(EnvelopeLocal *el, EnvelopeRemote *er)
{
    ASSERT (NULL != el);
    ASSERT (NULL != er);

    uint64 mlen = crypto_box_ZEROBYTES + sizeof(Time);  // Total message length including the zero offsets.
    static uchar tunnelIdCipherText[crypto_box_ZEROBYTES + sizeof(Time)];
    static uchar tunnelIdClearText [crypto_box_ZEROBYTES + sizeof(Time)];

    uint32_t seconds, nanoseconds;
    mixinGetTimeOfDay (&seconds, &nanoseconds);
    Time t = { seconds, nanoseconds };

    Nonce nonce[crypto_box_NONCEBYTES];
    _cryptoNonceGenerate(el, er);
    _cryptoNonceWrite(el->nonce, nonce);

    // Make room for zeros and then copy the cleartext message.
    memzero(tunnelIdClearText, crypto_box_ZEROBYTES);
    memcpy (tunnelIdClearText+crypto_box_ZEROBYTES, &t, sizeof(Time));

    // Encrypt the data.
    if(0 != cryptoBoxAfterNM(tunnelIdCipherText,
			     tunnelIdClearText,
                             mlen,
                             nonce,
                             er->symmetricKey))
	{
	    BUG();
	}

    _netRead64((char*) tunnelIdCipherText+crypto_box_BOXZEROBYTES, &er->cryptoTunnelId);
    _cryptoTunnelIdClearFirst(&er->cryptoTunnelId);

    return er->cryptoTunnelId;
}

//______________________________________________________________________________
// Create symmetric key, and also set oddSide and cryptoTunnelId.
//______________________________________________________________________________
bool
cryptoComputeSymmetricKey(EnvelopeLocal   *el,
			  EnvelopeRemote  *er,
                          NetCryptoKeyType keyType)
{
    ASSERT (NULL != el);
    ASSERT (NULL != er);

    bool fnval = false;
    const CryptoKey *localPublicKey  = NULL;
    const CryptoKey *localPrivateKey = NULL;

    // The one-time-use private key for outgoing tunnels is unnecessary after
    // computing the symmetric key, so it is not stored in EnvelopeLocal.
    // The public key must remain becuase we need to place it in the
    // tunnel's first packet.
    static CryptoKey ephemeralPrivateKeyForOutgoing[CryptoBoxPublicKeySize];

    switch (keyType)
	{
	case NetCryptoKeyPermanent:
	    localPublicKey  = el->publicKey;
	    localPrivateKey = el->privateKey;
	    break;
	case NetCryptoKeyEphemeralOutgoing:
	    // Generate a new ephemeral key pair for each outgoing connection.
	    cryptoBoxKeyPair (el->ephemeralPublicKeyForOutgoing, ephemeralPrivateKeyForOutgoing);
	    localPublicKey  = el->ephemeralPublicKeyForOutgoing;
	    localPrivateKey = ephemeralPrivateKeyForOutgoing;
	    break;
	case NetCryptoKeyEphemeralIncoming:
	    // This keypair is updated per the service record policy.
	    if (false == netInterfaceUpdateEphemeralKey ())
		{
		    // No ephemeral key generated yet or ephemeral key expired.
		    goto done;
		}
	    localPublicKey  = el->ephemeralPublicKeyForIncoming;
	    localPrivateKey = el->ephemeralPrivateKeyForIncoming;
	    break;
	default:
	    BUG ();
	}

    ASSERT (NULL != localPublicKey);
    ASSERT (NULL != localPrivateKey);

    // Remote and local public keys must not be the same.
    if (0 == memcmp(er->ephemeralPublicKey, localPublicKey, CryptoBoxPublicKeySize))
	{
	    goto done;
	}

    er->oddSide = memcmp(er->ephemeralPublicKey, localPublicKey, CryptoBoxPublicKeySize) > 0 ? true : false;

    if (0 != cryptoBoxBeforeNM(er->symmetricKey,
                               er->ephemeralPublicKey,
                               localPrivateKey))
	{
	    goto done;
	}

    if (0==er->cryptoTunnelId)
	{
	    _cryptoTunnelIdGenerate(el, er);
	}

    if (NetCryptoKeyEphemeralOutgoing == keyType)
	{
	    // Zero one-time-use private key after computing symmetric key.
	    memzero (ephemeralPrivateKeyForOutgoing, CryptoBoxPrivateKeySize);
	}
	    
    debugXPrint(tunnelDebug, "cryptoComputeSymmetricKey: symmetric $[symkey]\n", er->symmetricKey);
    debugXPrint(tunnelDebug, "cryptoComputeSymmetricKey: r public  $[pubboxkey]\n", er->ephemeralPublicKey);
    debugXPrint(tunnelDebug, "cryptoComputeSymmetricKey: l private $[privboxkey]\n", localPrivateKey);
    debugXPrint(tunnelDebug, "cryptoComputeSymmetricKey: l public  $[pubboxkey]\n", localPublicKey);
    debugXPrint(tunnelDebug, "cryptoComputeSymmetricKey: tunnelId  $[llhexint]\n", er->cryptoTunnelId);

    fnval = true;

done:
    return fnval;
}

//______________________________________________________________________________
// encrypt data
//______________________________________________________________________________
static
bool
_cryptoEncrypt(EnvelopeBuffer  payload,
	      EnvelopeBuffer  data,
	      EnvelopeLocal  *el,
	      EnvelopeRemote *er ///< encryption parameters
	      )
{
    ASSERT (NULL != el);
    ASSERT (NULL != er);

    bool fnval = false;

    uint64 mlen = crypto_box_ZEROBYTES + data.size;  // Total message length including the zero offsets.
    static ClearText clearText[EthernetMaxBufferSize];
    CipherText *cipherText = (CipherText*) payload.start-crypto_box_BOXZEROBYTES; // Make room for leading zeros which will be trimmed from cipherText.

    Nonce nonce[crypto_box_NONCEBYTES];
    _cryptoNonceGenerate(el, er);
    _cryptoNonceWrite(el->nonce, nonce);

    // Make room for zeros and then copy the cleartext message.
    memzero(clearText, crypto_box_ZEROBYTES);
    memcpy(clearText+crypto_box_ZEROBYTES, data.start, data.size);

    // Encrypt the data.
    if (cryptoBoxAfterNM(cipherText, clearText, mlen, nonce, er->symmetricKey) != 0) 
	{
	    debugXPrint(tunnelDebug, "cryptoEncrypt: Cannot box message\n");
	    goto done;
	}

    fnval = true;

done:
    return fnval;
}

//______________________________________________________________________________
// create encrypted data within payload
//______________________________________________________________________________
bool
cryptoCreateEncrypted(bool firstPacket,
                      EnvelopeBuffer *payload,       ///< will return the encrypted & header part of payload
		      EnvelopeBuffer  data,          ///< cleartext
		      EnvelopeLocal  *el,            ///< nonce
		      EnvelopeRemote *er             ///< encryption parameter
		      )
{
    ASSERT(NULL != payload);
    ASSERT(NULL != el);
    ASSERT(NULL != er);
    ASSERT(crypto_box_BOXZEROBYTES <= CryptoHeaderSize);
    ASSERT((crypto_box_BOXZEROBYTES + CryptoReserveSize) == crypto_box_ZEROBYTES);

    bool fnval = false;

    // crypto headers always start at crypto
    char *header = payload->start - CryptoHeaderSize;

    // size of encrypted payload is CryptoReserveSize bigger than cleartext data
    payload->size = data.size + CryptoReserveSize;

    if (firstPacket)
	{   // Make room for the public key in the header.
	    payload->start += CryptoBoxPublicKeySize;
	}

    if (! _cryptoEncrypt(*payload, data, el, er))
	{
	    goto done;
	}

    uint64 cryptoTunnelId  = er->cryptoTunnelId;
    
    if (firstPacket)
	{ // calculate the tunnel Id for a new tunnel
	    _cryptoTunnelIdSetFirst(&cryptoTunnelId);
	}
    
    payload->size  += payload->start - header;
    payload->start  = header;

    header = _netWrite64(header, cryptoTunnelId);
    //debugXPrint(envelopeDebug, "createCrypto:  el->nonce = $[lluint]  er->nonce = $[lluint]\n",
    //	     el->nonce, er->nonce);
    header = _netWrite64(header, el->nonce);

    if (firstPacket)
	{
	    // FIXME: Outgoing key must not have changed since corresponding cryptoComputeSymmetricKey!
	    header = _netWritePk(header, el->ephemeralPublicKeyForOutgoing);
	}

    fnval = true;

done:
    return fnval;
}

//______________________________________________________________________________
// Decrypt packet
//______________________________________________________________________________
bool
cryptoDecrypt(EnvelopeBuffer  payload,
	      EnvelopeBuffer *cleartext,
	      EnvelopeLocal  *el,
	      EnvelopeRemote *er,
	      Tunnel *tunnel)
{
    ASSERT(NULL != cleartext);
    ASSERT(NULL != el);
    ASSERT(NULL != er);
    ASSERT(NULL != tunnel);

    bool fnval = false;

    uint64 mlen = crypto_box_BOXZEROBYTES + payload.size;
    ASSERT(mlen <= EthernetMaxBufferSize);

    static CryptoKey cipherText[EthernetMaxBufferSize];
    Nonce nonce[crypto_box_NONCEBYTES];

    _cryptoNonceWrite(er->nonce, nonce);

    memzero(cipherText, crypto_box_BOXZEROBYTES);
    memcpy(cipherText+crypto_box_BOXZEROBYTES, payload.start, payload.size);

    if (cryptoBoxOpenAfterNM((uchar*) cleartext->start, cipherText, mlen,
				nonce,
                                tunnel->envelopeRemote.symmetricKey) != 0) 
        {
            debugXPrint(tunnelDebug, "cryptoDecrypt: cryptoBoxOpen() failed.\n");
	    goto done;
        }

    // skip over the zero bytes
    cleartext->start += crypto_box_ZEROBYTES;
    cleartext->size   = mlen - crypto_box_ZEROBYTES;

    fnval = true;

done:
    return fnval;
}

//______________________________________________________________________________
/// Extract tunnel key, nonce, and--if present---public key.
//______________________________________________________________________________
bool
cryptoExtract(EnvelopeBuffer *payload,
	      EnvelopeRemote *envelopeRemote)
{
    ASSERT(NULL != payload);
    ASSERT(NULL != envelopeRemote);

    bool fnval = false;
    char *buffer = payload->start;

    uint64 cryptoTunnelId;
    buffer = _netRead64(buffer, &cryptoTunnelId);
    buffer = _netRead64(buffer, &envelopeRemote->nonce);

    envelopeRemote->publicKeyPresent = _cryptoTunnelIdIsFirst(cryptoTunnelId);
    _cryptoTunnelIdClearFirst(&cryptoTunnelId);
    envelopeRemote->cryptoTunnelId = cryptoTunnelId;

    if (envelopeRemote->publicKeyPresent)
	{   // extract public key.
	    if (payload->size < (CryptoHeaderSize+CryptoBoxPublicKeySize))
		{   // No room in packet for expected public key.
		    goto done;
		}
	    
	    buffer = _netReadPk(buffer, envelopeRemote->ephemeralPublicKey);
	}

    payload->size -= buffer - payload->start;
    payload->start = buffer;

    fnval = true;

done:
    return fnval;
}

//______________________________________________________________________________
// Validate an authenticator
//
// The authenticator is the:
//   recipient's public encryption key
// encrypted using the:
//   recipient's public encryption key and senders private encryption key.
//
// Users are identified by their encryption key.
//______________________________________________________________________________
Status
cryptoAuthenticatorIsValid (PublicEncryptionKey  senderPubKey,
                            PublicEncryptionKey  recipientPubKey,
                            PrivateEncryptionKey recipientPrivKey,
                            Authenticator       *authenticator)
{
    ASSERT (NULL != authenticator);

    Status status = StatusOk;
    static Nonce nonce[crypto_box_NONCEBYTES];
    static ClearText  plainText [crypto_box_ZEROBYTES + sizeof (PublicEncryptionKey)];

    _cryptoNonceWrite(authenticator->nonce, nonce);

    if (0 != cryptoBoxOpen(plainText,
                           authenticator->authenticator,
                           sizeof (plainText),
                           nonce,
                           senderPubKey,
                           recipientPrivKey))
	{
	    debugXPrint(authenticateDebug, "decrypt failed\n");
	    status = StatusCertBadSignature;
	    goto done;
	}

    if (memcmp (plainText + crypto_box_ZEROBYTES, recipientPubKey, sizeof (PublicEncryptionKey)))
	{
	    debugXPrint(authenticateDebug, "compare failed\n");
	    status = StatusCertBadSignature;
	    goto done;
	}

done:
    return status;
}

//______________________________________________________________________________
// Generate an authenticator
//
// The authenticator is the:
//   recipient's public encryption key
// encrypted using the:
//   recipient's public encryption key and senders private encryption key.
//
// Users are identified by their encryption key.
//______________________________________________________________________________
Status
cryptoGenAuthenticator (PrivateEncryptionKey senderPrivKey,
                        PublicEncryptionKey  recipientPubKey,
                        Authenticator       *authenticator)
{
    ASSERT (NULL != authenticator);

    Status status = StatusOk;
    static uint64_t nextNonce = 0;
    static Nonce nonce[crypto_box_NONCEBYTES];
    static ClearText  plainText [crypto_box_ZEROBYTES + sizeof (PublicEncryptionKey)];

    memzero(plainText, crypto_box_ZEROBYTES);
    memcpy (plainText + crypto_box_ZEROBYTES, recipientPubKey, sizeof (PublicEncryptionKey));

    // FIXME: Is this a safe way to generate this nonce?
    authenticator->nonce = nextNonce++;
    _cryptoNonceWrite(authenticator->nonce, nonce);

    if (0 != cryptoBox(authenticator->authenticator,
                        plainText,
			sizeof (plainText),
                        nonce,
                        recipientPubKey,
                        senderPrivKey))
	{
	    status = StatusFail;
	    goto done;
	}

done:
    return status;
}

//_____________________________________________________________________________
// Check that the nonce has not expired and is greater than the last nonce 
// value received from the same host.
//______________________________________________________________________________
bool
cryptoNonceCheck(EnvelopeLocal  *el,
		 EnvelopeRemote *er,
		 uint64 nonce)
{
    ASSERT(NULL != el);
    ASSERT(NULL != er);

    bool fnval = false;

    // Check parity of received nonce.
    if (! (nonce & 1) ^ (er->oddSide))
	{
	    goto done;
	}

    // Check if nonce is greater than the last one received.
    if (nonce <= er->nonce)
	{
	    goto done;
	}

    /*
      uint64 nanosecs;
      Time time = getTimeOfDay() ;

      nanosecs = (uint64)(time.seconds * NanosecsPerSec + time.nanoseconds)  ;

      uint64  diff ;
      diff = (nanosecs > nonce) ? nanosecs - nonce : nonce - nanosecs;
      check time window.
      This test turned off for now.
      if (diff < NonceWindowSecs * NanosecsPerSec) 
          {  // This should be checked only for valid packets!!!
             debugXPrint(tunnelDebug, "time difference between the nonces correct = $[uint] and required = $[uint]\n", diff,  1000000000ull * 240);
             return 0 ;
          }
    */

    fnval = true;

done:
    return fnval;
}
