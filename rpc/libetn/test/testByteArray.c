#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	int i;
	ByteArray in;
	ByteArray out;

	for (i = 0; i < sizeof (ByteArray); i++) {
		in[i] = i % 255;
	}

	uint8_t buf[sizeof (uint32_t) + sizeof (ByteArray)]; // 32-bit length followed by data.
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof (buf));
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&ByteArrayType, &in), &etnEncodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&ByteArrayType, &out));

	free (e);
	etnBufferDecoderFree (d);

	return ! memcmp (in, out, sizeof (ByteArray));
}

UnitTest testByteArray = { __FILE__, _predicate };
