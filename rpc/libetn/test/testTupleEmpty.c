#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	bool fnval;
	String *in = stringEmpty (), *out;

	uint8_t buf[1024];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&Uint32TupleType, &in), &etnEncodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&Uint32TupleType, &out));

	fnval = ! stringCompare (in, out);

	free (e);
	etnBufferDecoderFree (d);
	refUnhook (&out);

	return fnval;
}

UnitTest testTupleEmpty = { __FILE__, _predicate };
