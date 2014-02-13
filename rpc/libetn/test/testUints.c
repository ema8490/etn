#include <stdlib.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	Uints in, out;

	in.field1 = 0xdeadbeef;
	in.field2 = 0xfeedface;
	in.field3 = 0x8badf00d;

	uint8_t buf[1024];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&UintsType, &in), &etnEncodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&UintsType, &out));

	free (e);
	etnBufferDecoderFree (d);

	return in.field1 == out.field1
	    && in.field2 == out.field2
	    && in.field3 == out.field3;
}

UnitTest testUints = { __FILE__, _predicate };
