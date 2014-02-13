#include <ethos/dual/ref.h>
#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

static const int TupleLen = 128;

static bool
_predicate (void)
{
	bool fnval;
	uint32_t ints[TupleLen];

	int i;
	for (i = 0; i < TupleLen; i++) {
		ints[i] = i;
	}

	String *in = stringAllocateEmpty(TupleLen * sizeof (uint32_t)), *out;
	xvectorIncrease(in->ptr, TupleLen * sizeof (uint32_t));
	memcpy (in->ptr, ints, TupleLen * sizeof (uint32_t));

	uint8_t buf[10240];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&Uint32TupleType, &in), &etnEncodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&Uint32TupleType, &out));

	fnval = ! memcmp (in->ptr, out->ptr, TupleLen * sizeof (uint32_t))
	     && refSize(in) == refSize(out);

	free (e);
	etnBufferDecoderFree (d);
	refUnhook (&in);
	refUnhook (&out);

	return fnval;
}

UnitTest testTuple = { __FILE__, _predicate };
