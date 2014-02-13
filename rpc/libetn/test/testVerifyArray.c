#include <stdlib.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	ByteArray data;
	uint16_t badData;
	// Note: you may find the hash for the ByteArray type using egPrint.
	uint8_t hash[] = { 0x98, 0x13, 0xe7, 0x9a, 0xb2, 0x6, 0xc5, 0x23, 0x64, 0x58, 0xc1, 0x42, 0xad, 0x2, 0x6e, 0x78, 0x17, 0x71, 0x30, 0xa8, 0xe3, 0xe3, 0xb5, 0x9, 0x15, 0x5a, 0x65, 0xa5, 0x97, 0x7a, 0xe7, 0x97, 0x53, 0xa9, 0xf0, 0x57, 0x20, 0xac, 0xc1, 0x4c, 0xb2, 0x41, 0xcd, 0xdb, 0xec, 0x86, 0x4e, 0x39, 0x50, 0x10, 0x2c, 0xf4, 0x8b, 0x6f, 0x6a, 0x42, 0x72, 0xcc, 0xda, 0xee, 0xec, 0x41, 0xef, 0x61 };

	Slice types;
	loadTypeGraph (&types, TestTypesPath);

	int i;
	for (i = 0; i < sizeof (ByteArray); i++) {
		data[i] = i % 255;
	}

	uint8_t buf[sizeof (EtnLength) + sizeof (ByteArray)]; // 32-bit length followed by data.
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));

	EtnLength etnEncodedSize;
	etnEncode ((EtnEncoder *) e, EtnToValue(&ByteArrayType, &data), &etnEncodedSize);
	EtnBufferVerifier *v = etnBufferVerifierNew(types, buf, etnEncodedSize);
	if (StatusOk != etnVerify ((EtnVerifier *) v, hash)) {
		return false;
	}

	etnBufferEncoderReset (e, buf, sizeof(buf));
	etnEncode ((EtnEncoder *) e, EtnToValue(&Uint16Type, &badData), &etnEncodedSize);
	etnBufferVerifierReset (v, buf, etnEncodedSize);
	if (StatusOk == etnVerify ((EtnVerifier *) v, hash)) {
		return false;
	}

	free (e);
	free (v);

	return true;
}

UnitTest testVerifyArray = { __FILE__, _predicate };
