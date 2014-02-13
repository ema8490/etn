#include <stdlib.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	uint8_t *in = malloc (sizeof (uint8_t)), *out;
	*in = 0xbe;

	uint8_t buf[1024];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnLength encodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&Uint8PtrType, &in), &encodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, encodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&Uint8PtrType, &out));

	free (e);
	etnBufferDecoderFree (d);

	return *in == *out;
}

UnitTest testPointer = { __FILE__, _predicate };
