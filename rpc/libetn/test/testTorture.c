#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

static const char *helloWorld = "Hello, world!";

static bool
_predicate (void)
{
	bool fnval;
	Torture in, out;

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
	in.int8Ptr = &in.int8; // Not technically legal to point into struct.
	in.tuple = stringAllocateInitialize("Hello, world!");
	strcpy ((char *) in.array, helloWorld);

	uint8_t buf[sizeof (Torture) + refSize(in.tuple)];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&TortureType, &in), &etnEncodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&TortureType, &out));

	fnval = in.int8     == out.int8
	     && in.uint8    == out.uint8
	     && in.uint16   == out.uint16
	     && in.uint16   == out.uint16
	     && in.uint32   == out.uint32
	     && in.uint32   == out.uint32
	     && in.uint64   == out.uint64
	     && in.uint64   == out.uint64
//	     && in.float32  == out.float32
//	     && in.float64  == out.float64
//	     && in.bool     == out.bool
	     && *in.int8Ptr == *out.int8Ptr
	     && ! stringCompare (in.tuple, out.tuple)
	     && ! strcmp ((char *) out.array, helloWorld);

	free (e);
	etnBufferDecoderFree (d);
	stringUnhook (&in.tuple);
	stringUnhook (&out.tuple);

	return fnval;
}

UnitTest testTorture = { __FILE__, _predicate };
