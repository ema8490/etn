#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	bool fnval;
	Torture in;

	in.int8 = 0;
	in.uint8 = 1;
	in.int16 = 2;
	in.uint16 = 3;
	in.int32 = 4;
	in.uint32 = 5;
	in.int64 = 6;
	in.uint64 = 7;
//	in.float32 = 3.14;
//	in.float64 = 3.1415;
//	in.bool = false;
	in.int8Ptr = &in.int8;
	in.tuple = stringAllocateInitialize("Hello, world!");
	strcpy ((char *) in.array, HelloWorld);

	EtnLength controlEncodedSize =
	         sizeof (in.int8)
	       + sizeof (in.uint8)
	       + sizeof (in.int16)
	       + sizeof (in.uint16)
	       + sizeof (in.int32)
	       + sizeof (in.uint32)
	       + sizeof (in.int64)
	       + sizeof (in.uint64)
	       + sizeof (in.float32)
	       + sizeof (in.float64)
	       + 1 + sizeof (uint32_t) // Pointer: pointer encoding type + index.
	       + sizeof (uint32_t) + refSize (in.tuple) // Tuple: length + data.
	       + sizeof (in.array); // Array: data.

	EtnNullEncoder *e = etnNullEncoderNew();
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&TortureType, &in), &etnEncodedSize);

	// Add sizeof (uint32_t) for Tuple's length encoding.
	fnval = etnEncodedSize == controlEncodedSize;

	free (e);

	return fnval;
}

UnitTest testSizeTorture = { __FILE__, _predicate };
