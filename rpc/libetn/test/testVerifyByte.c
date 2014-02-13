#include <stdlib.h>

#include "unitTest.h"

static bool
_predicate (void)
{
	uint8_t data;
	int16_t badData;
	// Note: you may find the hash for the byte type using egPrint.
	uint8_t hash[] = { 0x09, 0x66, 0x78, 0xbb, 0x4a, 0x86, 0x63, 0x8d, 0x8e, 0xc6, 0x58, 0x8e, 0xcc, 0x2d, 0x89, 0x5a, 0x5c, 0x17, 0xb4, 0x86, 0x37, 0x8c, 0x81, 0xc2, 0xf2, 0xac, 0xf2, 0x48, 0x67, 0x57, 0x82, 0x13, 0x3e, 0x2e, 0x7d, 0x80, 0xb6, 0x66, 0x8d, 0x84, 0xc4, 0x55, 0xf4, 0xc9, 0xe1, 0x33, 0xfc, 0x71, 0x0b, 0x77, 0x43, 0x63, 0x06, 0x82, 0x76, 0x63, 0x07, 0xbc, 0xf1, 0x64, 0xdb, 0xd9, 0x5c, 0x93 };

	Slice types;
	loadTypeGraph (&types, TestTypesPath);

	uint8_t buf[1024];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));

	EtnLength etnEncodedSize;
	etnEncode ((EtnEncoder *) e, EtnToValue(&Uint8Type, &data), &etnEncodedSize);
	EtnBufferVerifier *v = etnBufferVerifierNew(types, buf, etnEncodedSize);
	if (StatusOk != etnVerify ((EtnVerifier *) v, hash)) {
		return false;
	}

	etnBufferEncoderReset (e, buf, sizeof(buf));
	etnEncode ((EtnEncoder *) e, EtnToValue(&Uint16Type, &badData), &etnEncodedSize);
	etnBufferVerifierReset(v, buf, etnEncodedSize);
	if (StatusOk == etnVerify ((EtnVerifier *) v, hash)) {
		return false;
	}

	free (e);
	free (v);

	return true;
}

UnitTest testVerifyByte = { __FILE__, _predicate };
