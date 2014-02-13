#include <ethos/dual/ref.h>
#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	bool fnval;
	StringAndAny in, out;

	uint32_t data = 0xdeadbeef;
	EtnType *t = &Uint32Type;
	EtnValue v = (EtnValue) { t, &data };

	// Note: you may find the hash for the Uint32Type type in builtins.go.
	uint8_t h[] = { 0xce, 0xf4, 0x3a, 0x5, 0xae, 0x67, 0xd9, 0x73, 0xc2, 0xa2, 0x1d, 0xf8, 0xcd, 0xf9, 0xd2, 0xde, 0x69, 0x8d, 0xd, 0xb7, 0x61, 0xb9, 0x51, 0x22, 0x58, 0xed, 0x8f, 0xb1, 0x83, 0xf1, 0x5c, 0xff, 0x5b, 0x84, 0xe2, 0x14, 0xe, 0x10, 0x68, 0x3f, 0x7a, 0xd9, 0xa7, 0x8f, 0x5b, 0xe4, 0x9e, 0x4e, 0x0, 0x7d, 0xcb, 0xfb, 0xd1, 0x69, 0x59, 0x9d, 0xbf, 0x9b, 0x75, 0x65, 0x15, 0x9e, 0x8b, 0x82 };
	uint8_t *h1 = malloc (sizeof(h));
	memcpy (h1, h, sizeof (h));

	in.field1 = stringAllocateInitialize("Hello, world!");
	in.field2 = v;

	etnRegisterType (t, h1);

	uint8_t buf[1024];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&StringAndAnyType, &in), &etnEncodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&StringAndAnyType, &out));

	fnval = ! stringCompare (in.field1, out.field1)
             && in.field2.type->kind == out.field2.type->kind
             && *(uint32_t *) in.field2.data == *(uint32_t *) out.field2.data;

	free (e);
	etnBufferDecoderFree (d);
	refUnhook (&out.field1);
	free (out.field2.data);
	free (h1);

	return fnval;
}

UnitTest testStringAndAny = { __FILE__, _predicate };
