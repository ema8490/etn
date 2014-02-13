#include <ethos/dual/ref.h>
#include <stdlib.h>
#include <string.h>

#include "unitTest.h"

static bool
_predicate (void)
{
	bool fnval;
	String *in = stringAllocateInitialize("Hello, world!"), *out;

	uint8_t buf[1024];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&StringType, &in), &etnEncodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&StringType, &out));

	fnval = ! stringCompare (in, out);

	free (e);
	etnBufferDecoderFree (d);
	refUnhook (&in);
	refUnhook (&out);

	return fnval;
}

UnitTest testString = { __FILE__, _predicate };
