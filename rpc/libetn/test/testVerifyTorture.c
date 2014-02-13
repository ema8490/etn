#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

static const char *helloWorld = "Hello, world!";

static bool
_predicate (void)
{
	Torture data;
	// Note: you may find the hash for the Torture type using egPrint.
	uint8_t hash[] = { 0xe0, 0x73, 0xd1, 0xc1, 0x23, 0x8e, 0x88, 0xbf, 0xe2, 0x69, 0xcb, 0xc4, 0x4d, 0xe7, 0xb7, 0x63, 0x86, 0x76, 0x1c, 0x71, 0x79, 0x59, 0x2, 0x4d, 0x2f, 0x77, 0x90, 0x38, 0x41, 0x23, 0x14, 0x7d, 0xec, 0xfd, 0x8a, 0x49, 0xf, 0xfb, 0x3, 0x7d, 0xa7, 0x2b, 0x86, 0x73, 0xe2, 0xa0, 0x4a, 0x17, 0x24, 0x91, 0x69, 0x30, 0x16, 0x19, 0xb3, 0x78, 0xe3, 0x88, 0x6, 0xd5, 0x3e, 0xa7, 0xa2, 0x37 };

	Slice types;
	loadTypeGraph (&types, TestTypesPath);

	const char bytes[] = "Hello, world!";

	data.int8 = 0;
	data.uint8 = 1;
	data.int16 = 2;
	data.uint16 = 3;
	data.int32 = 4;
	data.uint32 = 5;
	data.int64 = 6;
	data.uint64 = 7;
//	data.float32 = 3.14;
//	data.float64 = 3.1415;
//	data.bool = false;
	data.int8Ptr = &data.int8;
	data.tuple = stringAllocateInitialize(bytes);
	strcpy ((char *) data.array, helloWorld);

	uint8_t buf[sizeof (Torture) + sizeof (bytes)];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));

	EtnLength encodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&TortureType, &data), &encodedSize);
	EtnBufferVerifier *v = etnBufferVerifierNew(types, buf, encodedSize);
	if (StatusOk != etnVerify ((EtnVerifier *) v, hash)) {
		return false;
	}

	free (e);
	free (v);

	return true;
}

UnitTest testVerifyTorture = { __FILE__, _predicate };
