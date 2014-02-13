#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	bool fnval;
	Strings in1, out1;
	StructContainingTuple in2, out2;

	in1.field1 = stringAllocateInitialize("Hello, world!");
	in1.field2 = stringAllocateInitialize("Goodbye, cruel world!");

	in2.strings = stringAllocateEmpty(sizeof (Strings));
	stringConcat (in2.strings, &in1, sizeof (Strings));

	uint8_t buf[1024];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&StructContainingTupleType, &in2), &etnEncodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&StructContainingTupleType, &out2));

	out1 = *(Strings *) out2.strings->ptr;

	fnval = ! stringCompare (in1.field1, out1.field1)
	     && ! stringCompare (in1.field2, out1.field2);
	
	free (e);
	etnBufferDecoderFree (d);
	stringUnhook (&in1.field1);
	stringUnhook (&in1.field2);
	stringUnhook (&in2.strings);
	stringUnhook (&out1.field1);
	stringUnhook (&out1.field2);
	stringUnhook (&out2.strings);

	return fnval;
}

UnitTest testStructContainingTuple = { __FILE__, _predicate };
