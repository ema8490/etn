#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	TestUnion in;

	in.smallInteger   = 0xdeadbeef;
	EtnValue value1   = (EtnValue) { &Uint32Type, &in.smallInteger };
	EtnValue value2   = (EtnValue) { &Uint64Type, &in.largeInteger };

	// Note: you may find the hash for the TestUnion type using egPrint.
	uint8_t hash[] = { 0xbe, 0xdb, 0x2b, 0x78, 0x27, 0x9c, 0x2a, 0x58, 0x9f, 0xfe, 0x6b, 0x1b, 0x25, 0xd9, 0x29, 0xd4, 0x17, 0xef, 0x48, 0x4, 0x43, 0x12, 0x8e, 0x18, 0x40, 0x2d, 0x79, 0xae, 0x6b, 0x53, 0x2d, 0xed, 0xd9, 0x47, 0x1c, 0x4b, 0x30, 0x90, 0x1b, 0xbb, 0x9b, 0x7e, 0x77, 0xec, 0x4c, 0x33, 0x8e, 0x76, 0x32, 0x9b, 0xff, 0x52, 0x18, 0x9d, 0xa2, 0xcf, 0xfd, 0xeb, 0x43, 0xdb, 0x4c, 0x99, 0x83, 0x6d };

	Slice types;
	loadTypeGraph (&types, TestTypesPath);

	uint8_t buf[1024];

	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnLength encodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&TestUnionType, &value1), &encodedSize);
	EtnBufferVerifier *v = etnBufferVerifierNew(types, buf, encodedSize);
	if (StatusOk != etnVerify ((EtnVerifier *) v, hash)) {
		return false;
	}

	etnBufferEncoderReset(e, buf, sizeof(buf));
	etnEncode((EtnEncoder *) e, EtnToValue(&TestUnionType, &value2), &encodedSize);
	etnBufferVerifierReset(v, buf, encodedSize);
	if (StatusOk != etnVerify ((EtnVerifier *) v, hash)) {
		return false;
	}

	free (e);
	free (v);

	return true;
}

UnitTest testVerifyUnion = { __FILE__, _predicate };
