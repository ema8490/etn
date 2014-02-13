#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	uint32_t i;
	Uint32Array in;
	Uint32Array out;

	for (i = 0; i < sizeof (Uint32Array) / sizeof (uint32_t); i++) {
		in[i] = i;
	}

	uint8_t buf[sizeof (EtnLength) + sizeof (Uint32Array)]; // 32-bit length followed by data.
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof (buf));
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&Uint32ArrayType, &in), &etnEncodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&Uint32ArrayType, &out));

	free (e);
	etnBufferDecoderFree (d);

	return ! memcmp (in, out, sizeof (Uint32Array));
}

UnitTest testUint32Array = { __FILE__, _predicate };
