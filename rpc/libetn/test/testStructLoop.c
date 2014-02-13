#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	LoopA a, out;
	LoopB b;
	LoopC c;

	a.b = &b;
	a.message = stringAllocateInitialize("Hello, world!");
	b.c = &c;
	c.a = &a;

	uint8_t buf[1024];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&LoopAType, &a), &etnEncodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&LoopAType, &out));

	free (e);
	etnBufferDecoderFree (d);

	// Ensure c's pointer points back to a and verify Message just to be sure.
	return (void *) &out  == (void *) out.b->c->a
	    && (void *) &out  != (void *) out.b
	    && (void *) out.b != (void *) out.b->c
	    && ! stringCompare (out.message, out.b->c->a->message);
}

UnitTest testStructLoop = { __FILE__, _predicate };
