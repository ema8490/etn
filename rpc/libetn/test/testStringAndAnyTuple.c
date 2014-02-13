#include <ethos/dual/ref.h>
#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	bool fnval;
	StringAndAnyTuple in, out;

	EtnType *t = &Uint32Type;
	EtnValue node[2];
	uint32_t data = 0xdeadbeef;
	node[0] = (EtnValue) { t, &data };
	node[1] = (EtnValue) { t, &data };
	String *tuple = stringAllocate (2 * sizeof (EtnValue));
	stringConcat (tuple, &node[0], sizeof (EtnValue));
	stringConcat (tuple, &node[1], sizeof (EtnValue));

	// Note: you may find the hash for the Uint32Type type in builtins.go.
	uint8_t h[] = { 0xce, 0xf4, 0x3a, 0x5, 0xae, 0x67, 0xd9, 0x73, 0xc2, 0xa2, 0x1d, 0xf8, 0xcd, 0xf9, 0xd2, 0xde, 0x69, 0x8d, 0xd, 0xb7, 0x61, 0xb9, 0x51, 0x22, 0x58, 0xed, 0x8f, 0xb1, 0x83, 0xf1, 0x5c, 0xff, 0x5b, 0x84, 0xe2, 0x14, 0xe, 0x10, 0x68, 0x3f, 0x7a, 0xd9, 0xa7, 0x8f, 0x5b, 0xe4, 0x9e, 0x4e, 0x0, 0x7d, 0xcb, 0xfb, 0xd1, 0x69, 0x59, 0x9d, 0xbf, 0x9b, 0x75, 0x65, 0x15, 0x9e, 0x8b, 0x82 };
	uint8_t *h1 = malloc (sizeof(h));
	memcpy (h1, h, sizeof (h));

	in.field1 = stringAllocateInitialize("Hello, world!");
	in.field2 = tuple;

	etnRegisterType (t, h1);

	uint8_t buf[1024];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&StringAndAnyTupleType, &in), &etnEncodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&StringAndAnyTupleType, &out));

	// FAILS!
	// FIXME: refSize (in.field1) == 13, but refSize (in.field2) == 32!
	//        because _decodeTuple creates a string big enough to hold
	//        len * sizeof (ANY TYPE), not len * sizeof (ACTUAL TYPE)!
	fnval = ! stringCompare (in.field1, out.field1)
             && refSize (in.field1) == refSize (in.field2)
             && ((uint32_t *) in.field2->ptr)[0] == ((uint32_t *) out.field2->ptr)[0]
             && ((uint32_t *) in.field2->ptr)[1] == ((uint32_t *) out.field2->ptr)[1];

	free (e);
	etnBufferDecoderFree (d);
	refUnhook (&out.field1);
	refUnhook (&out.field2);
	free (h1);

	return fnval;
}

UnitTest testStringAndAnyTuple = { __FILE__, _predicate };
