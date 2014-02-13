//______________________________________________________________________________
// Wrapper for crypto operations in Ethos kernel
//
// Jan-2013: Xu Zhang
//______________________________________________________________________________

#include <ethos/dual/core.h>
#include <ethos/generic/assert.h>
#include <ethos/generic/crypto.h>

static unsigned long long randomBytePoolAvailable = 0;
static unsigned char      randomBytePool[RandomBytePoolMaxRequest];

void mixinGetRandomBytes (unsigned char *x, unsigned long long xlen);

//______________________________________________________________________________
/// NaCl-require function that produces xlen random bytes and copies them
/// to an array starting at x. We require that sufficient random bytes are
/// generated (and stored in randomBytePool) *before* calling an NaCl function.
/// Our NaCl wrappers ensure this.
///
/// The reason we want to generate random data before calling NaCl functions
/// has to do with using libnetStack in the Ethos kernel. Here we might 
/// otherwise have to schedule a user-space process while waiting for the random
/// RPC. We would like to do this before making use of the FPU in an NaCl
/// function call.
//______________________________________________________________________________
void
randombytes(unsigned char *x, unsigned long long xlen)
{
    ASSERT (RandomBytePoolMaxRequest >= xlen); // Check separately to ensure
                                               // no future NaCl use exceeds.
    ASSERT (randomBytePoolAvailable  >= xlen);

    memcpy (x, randomBytePool, xlen);
    randomBytePoolAvailable -= xlen;
}

//______________________________________________________________________________
/// Produced xlen random bytes and copies them to an array starting at x, so
/// that sufficient random bytes are generated (and stored in randomBytePool)
/// *before* calling an NaCl function which needs randomness.
///
/// This ensures that no blocking is necessary within an NaCL function.
/// Our NaCl wrappers call this fucntions, with the maximum xlen.
//______________________________________________________________________________
void
cryptoPutRandomBytesToPool (unsigned long long xlen)
{
    ASSERT (RandomBytePoolMaxRequest >= xlen); // Check separately to ensure
                                               // no future NaCl use exceeds.
    ASSERT (RandomBytePoolMaxRequest - randomBytePoolAvailable >= xlen);

    mixinGetRandomBytes (randomBytePool, xlen);
    randomBytePoolAvailable += xlen;
}
