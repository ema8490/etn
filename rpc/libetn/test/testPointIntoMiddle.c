#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	Buffer buffer, out;

	buffer.buf = stringAllocateInitialize("0123456789");
	buffer.current = &((uint8_t *) buffer.buf->ptr)[5];

	uint8_t buf[1024];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnLength etnEncodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&BufferType, &buffer), &etnEncodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, etnEncodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&BufferType, &out));

	free (e);
	etnBufferDecoderFree (d);

	return ! stringCompare (buffer.buf, out.buf)
	    &&  out.current == &((uint8_t *) buffer.buf->ptr)[5]
	    && *out.current == '5';
}

UnitTest testPointIntoMiddle = { __FILE__, _predicate };
