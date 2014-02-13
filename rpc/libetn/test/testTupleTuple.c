#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

static const int TupleLen = 128;

static bool
_predicate (void)
{
	bool fnval = true;
	uint32_t ints[TupleLen];
	TupleTuple tuples[TupleLen];

	int i;
	for (i = 0; i < TupleLen; i++) {
		ints[i] = i;
	}

	String *intermediate = stringAllocateEmpty(sizeof (ints[0]) * TupleLen);
	stringConcat (intermediate, ints, sizeof (ints[0]) * TupleLen);

	for (i = 0; i < TupleLen; i++) {
		tuples[i] = intermediate;
	}

	String *in = stringAllocateEmpty(sizeof (tuples[0]) * TupleLen), *out;
	stringConcat (in, tuples, sizeof (tuples[0]) * TupleLen);

	uint8_t buf[1024 * 1024];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&TupleTupleType, &in), &etnEncodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&TupleTupleType, &out));

	int j;
	for (i = 0; i < TupleLen; i++) {
		for (j = 0; j < TupleLen; j++) {
			if (j != ((uint32_t *) ((String **) out->ptr)[i]->ptr)[j]) {
				fnval = false;
				goto done;
			}
		}
	}

done:
	free (e);
	etnBufferDecoderFree (d);
	stringUnhook (&in);
	stringUnhook (&out);

	return fnval;
}

UnitTest testTupleTuple = { __FILE__, _predicate };
