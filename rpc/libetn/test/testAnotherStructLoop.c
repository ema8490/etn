#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

// Another loop. This time the item etnEncoded is not part of the loop; we
// encode head instead of a. So, we have:
//
// head -> a -> b -> c
//         ^         |
//         -----------

static bool
_predicate (void)
{
	LoopHead head, out;
	LoopA a;
	LoopB b;
	LoopC c;

	head.a = &a;
	a.b = &b;
	a.message = stringAllocateInitialize ("Hello, world!");
	b.c = &c;
	c.a = &a;

	uint8_t buf[1024];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&LoopHeadType, &head), &etnEncodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&LoopHeadType, &out));

	free (e);
	etnBufferDecoderFree (d);

	// Ensure c's pointer points back to a and verify Message just to be sure.
	return (void *) out.a    == (void *) out.a->b->c->a
	    && (void *) &out     != (void *) out.a
	    && (void *) &out.a   != (void *) out.a->b
	    && (void *) out.a->b != (void *) out.a->b->c
	    && ! stringCompare (out.a->message, out.a->b->c->a->message);
}

UnitTest testAnotherStructLoop = { __FILE__, _predicate };
