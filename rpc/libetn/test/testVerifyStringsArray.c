#include <stdlib.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	int i;
	StringsArray in;
	// Note: you may find the hash for the StringsArray type using egPrint.
	uint8_t hash[] = { 0x11, 0xa3, 0xbf, 0x5d, 0x4e, 0x93, 0x82, 0x30, 0xa4, 0x46, 0x1b, 0xe6, 0x31, 0x60, 0x42, 0x4e, 0x64, 0xb8, 0xe8 , 0x6a, 0x45, 0x39, 0xd1, 0x42, 0x27, 0x8d, 0xac, 0x61, 0xa2, 0xc2, 0x77, 0xc0, 0xbc, 0x80, 0xc2, 0xea, 0x1d, 0xee, 0x6a, 0x43, 0x20, 0x36, 0x5c, 0xd7, 0xa0, 0x30, 0x57, 0x92, 0x89, 0x58, 0x93, 0x2c, 0xab, 0xc5, 0x21, 0x99, 0x7b, 0xb0, 0xac, 0x44, 0xe7, 0xb8, 0x45, 0x75 };

	Slice types;
	loadTypeGraph (&types, TestTypesPath);

	const char bytes1[] = "Hello, world!";
	const char bytes2[] = "Goodbye, cruel world!";

	for (i = 0; i < sizeof (StringsArray) / sizeof (Strings); i++) {
		in[i].field1 = stringAllocateInitialize(bytes1);
		in[i].field2 = stringAllocateInitialize(bytes2);
	}

	// 32-bit length followed by data.
	uint8_t buf[sizeof (EtnLength) + sizeof (StringsArray) * (sizeof (bytes1) + sizeof (bytes2))];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof (buf));
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&StringsArrayType, &in), &etnEncodedSize);

	EtnBufferVerifier *v = etnBufferVerifierNew(types, buf, etnEncodedSize);
	if (StatusOk != etnVerify ((EtnVerifier *) v, hash)) {
		return false;
	}

	free (e);
	free (v);

	return true;
}

UnitTest testVerifyStringsArray = { __FILE__, _predicate };
