#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	bool fnval = true;
	TestUnion testUnion;

	testUnion.smallInteger = 0xdeadbeef;
	EtnValue in1 = (EtnValue) { &Uint32Type, &testUnion.smallInteger }, out1;

	uint8_t buf[1024];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&TestUnionType, &in1), &etnEncodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&TestUnionType, &out1));

	if ((*(uint32_t *) in1.data) != (*(uint32_t *) out1.data)) {
		return false;
	}

	testUnion.largeInteger = 0xdeadbeefdeadbeef;
	EtnValue in2 = (EtnValue) { &Uint64Type, &testUnion.largeInteger }, out2;

	etnBufferEncoderReset(e, buf, sizeof(buf));
	etnEncode((EtnEncoder *) e, EtnToValue(&TestUnionType, &in2), &etnEncodedSize);

	etnBufferDecoderReset(d, buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&TestUnionType, &out2));

	if ((*(uint64_t *) in2.data) != (*(uint64_t *) out2.data)) {
		return false;
	}

	testUnion.smallInteger = 0xdeadbeef;
	StructWithUnion in3, out3;

	in3.s = stringAllocateInitialize("Hello, world!");
	in3.u = (EtnValue) { &Uint32Type, &testUnion };

	etnBufferEncoderReset(e, buf, sizeof(buf));
	etnEncode((EtnEncoder *) e, EtnToValue(&StructWithUnionType, &in3), &etnEncodedSize);

	etnBufferDecoderReset(d, buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&StructWithUnionType, &out3));

	fnval = ! stringCompare (in3.s, out3.s)
	     && ! stringCompare (in3.s, out3.s)
	     && ! memcmp (in3.u.data, out3.u.data, sizeof (testUnion.smallInteger));

	free (e);
	etnBufferDecoderFree (d);

	return fnval;
}

UnitTest testUnion = { __FILE__, _predicate };
