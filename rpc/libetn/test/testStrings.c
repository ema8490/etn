#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	bool fnval;
	Strings in, out;

	in.field1 = stringAllocateInitialize("Hello, world!");
	in.field2 = stringAllocateInitialize ("Goodbye, cruel world!");

	uint8_t buf[1024];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&StringsType, &in), &etnEncodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&StringsType, &out));

	fnval = ! stringCompare (in.field1, out.field1)
             && ! stringCompare (in.field2, out.field2);
	
	free (e);
	etnBufferDecoderFree (d);
	stringUnhook (&in.field1);
	stringUnhook (&in.field2);
	stringUnhook (&out.field1);
	stringUnhook (&out.field2);

	return fnval;
}

UnitTest testStrings = { __FILE__, _predicate };
