//______________________________________________________________________________
// Wrapper for crypto operations in Ethos kernel
//
// Jan-2013: Xu Zhang
//______________________________________________________________________________

#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#include <ethos/generic/mixin.h>
#include <nacl/crypto_box.h>
#include <nacl/crypto_hash.h>
#include <nacl/crypto_sign.h>
#include <nacl/randombytes.h>

enum {
        RandomBytePoolMaxRequest = CryptoSignPrivateKeySize // The maximum number of random bytes
                                                            // any NaCl function call we use will require.
};

void cryptoPutRandomBytesToPool (unsigned long long xlen);

static
inline
int
cryptoBoxKeyPair(unsigned char *publicKey,
		unsigned char *privateKey)
{
    cryptoPutRandomBytesToPool (CryptoBoxPrivateKeySize);
    mixinFloatingPointStateSave();

    return crypto_box_keypair(publicKey, privateKey);
}

static
inline
int
cryptoBox(unsigned char *cipherText,
		const unsigned char *plainText,
		unsigned long long plainTextLen,
		const unsigned char *nonce,
		const unsigned char *publicKey,
		const unsigned char *privateKey)
{
    mixinFloatingPointStateSave();

    return crypto_box(cipherText, plainText, plainTextLen, nonce, publicKey, privateKey);
}

static
inline
int
cryptoBoxOpen(unsigned char *plainText,
		const unsigned char *cipherText,
		unsigned long long cipherTextLen,
		const unsigned char *nonce,
		const unsigned char *publicKey,
		const unsigned char *privateKey)
{
    mixinFloatingPointStateSave();

    return crypto_box_open(plainText, cipherText, cipherTextLen, nonce, publicKey, privateKey);
}

static
inline
int
cryptoBoxBeforeNM(unsigned char *symmetricKey,
		const unsigned char *publicKey,
		const unsigned char *privateKey)
{
    mixinFloatingPointStateSave();

    return crypto_box_beforenm(symmetricKey, publicKey, privateKey);
}

static
inline
int
cryptoBoxAfterNM(unsigned char *cipherText,
		const unsigned char *plainText,
		unsigned long long plainTextLen,
		const unsigned char *nonce,
		const unsigned char *symmetricKey)
{
    mixinFloatingPointStateSave();

    return crypto_box_afternm(cipherText, plainText, plainTextLen, nonce, symmetricKey);
}

static
inline
int
cryptoBoxOpenAfterNM(unsigned char *plainText,
		const unsigned char *cipherText,
		unsigned long long cipherTextLen,
		const unsigned char *nonce,
		const unsigned char *symmetricKey)
{
    mixinFloatingPointStateSave();

    return crypto_box_open_afternm(plainText, cipherText, cipherTextLen, nonce, symmetricKey);
}

static
inline
int
cryptoHash(unsigned char *hash,
		const unsigned char *plainText,
		unsigned long long plainTextLen)
{
    mixinFloatingPointStateSave();

    return crypto_hash(hash, plainText, plainTextLen);
}

static
inline
int
cryptoSign(unsigned char *signature,
		unsigned long long *sigatureLen,
		const unsigned char *plainText,
		unsigned long long plainTextLen,
		const unsigned char *privateKey)
{
    mixinFloatingPointStateSave();

    return crypto_sign(signature, sigatureLen, plainText, plainTextLen, privateKey);
}

static
inline
int
cryptoSignOpen(unsigned char *plainText,
		unsigned long long *plainTextLen,
		const unsigned char *signature,
		unsigned long long sigatureLen,
		const unsigned char *publicKey)
{
    mixinFloatingPointStateSave();

    return crypto_sign_open(plainText, plainTextLen, signature, sigatureLen, publicKey);
}

static
inline
int
cryptoSignKeyPair(unsigned char *publicKey,
		unsigned char *privateKey)
{
    cryptoPutRandomBytesToPool (CryptoSignPrivateKeySize);
    mixinFloatingPointStateSave();

    return crypto_sign_keypair(publicKey, privateKey);
}

#endif
