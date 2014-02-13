#include <ethos/dual/ref.h>
#include <stdlib.h>
#include <string.h>

#include "unitTest.h"

enum {
	BigSize = 1024 * 1024 * 63, // Will allocate 2^n to satisfy size + 1 for strings;
                                    // default pool available for (x)malloc is 512MB.
	EncodedSize = sizeof (uint32_t) + BigSize // Encoding is length + slice.
};

static bool
_predicate (void)
{
	bool fnval;

	char *bytes = malloc (BigSize);
	if (! bytes) {
		return false;
	}

	int i;
	for (i = 0; i < BigSize; i++) {
		bytes[i] = 'a';
	}
	bytes[i] = 0x00;

	unsigned char *buf = malloc (EncodedSize);
	if (! buf) {
		return false;
	}

	String *in = stringAllocateInitialize (bytes), *out = NULL;

	EtnBufferEncoder *e = etnBufferEncoderNew(buf, EncodedSize);
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&StringType, &in), &etnEncodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&StringType, &out));
	ASSERT (NULL != out);

	fnval = ! stringCompare (in, out);

	free (e);
	free (buf);
	etnBufferDecoderFree (d);
	stringUnhook (&in);
	stringUnhook (&out);

	return fnval;
}

UnitTest testBigString = { __FILE__, _predicate };
