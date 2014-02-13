#include <stdlib.h>

#include "unitTest.h"

static bool
_predicate (void)
{
	uint32_t in = 0xdeadbeef, out;

	uint8_t buf[1024];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnLength encodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&Uint32Type, &in), &encodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, encodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&Uint32Type, &out));

	free (e);
	etnBufferDecoderFree (d);

	return in == out;
}

UnitTest testAdam = { __FILE__, _predicate };
