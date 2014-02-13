#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	int i;
	bool fnval = true;
	StringsArray in;
	StringsArray out;

	unsigned char bytes1[] = "Hello, world!";
	unsigned char bytes2[] = "Goodbye, cruel world!";

	for (i = 0; i < sizeof (StringsArray) / sizeof (Strings); i++) {
		in[i].field1 = stringAllocateInitialize("Hello, world!");
		in[i].field2 = stringAllocateInitialize("Goodbye, cruel world!");
	}

	// 32-bit length followed by data.
	uint8_t buf[sizeof (uint32_t) + sizeof (StringsArray) * (sizeof (bytes1) + sizeof (bytes2))];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof (buf));
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&StringsArrayType, &in), &etnEncodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&StringsArrayType, &out));

	for (i = 0; i < sizeof (StringsArray) / sizeof (Strings); i++) {
		if (stringCompare (in[i].field1, out[i].field1)
                 || stringCompare (in[i].field2, out[i].field2)) {
		 	fnval = false;
			goto done;
		 }
	}

done:
	free (e);
	etnBufferDecoderFree (d);

	return fnval;
}

UnitTest testStringsArray = { __FILE__, _predicate };
