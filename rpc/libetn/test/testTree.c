#include <ethos/generic/assert.h>
#include <stdlib.h>

#include "unitTest.h"
#include "treeFns.h"

static bool
_predicate (void)
{
	TreeNode *in = treeBuild (TreeHeight), out;

	uint8_t buf[1024 * sizeof (TreeNode)];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnLength encodedSize;
	etnEncode((EtnEncoder *) e, EtnToValue(&TreeNodeType, in), &encodedSize);

	EtnBufferDecoder *d = etnBufferDecoderNew(buf, encodedSize);
	etnDecode((EtnDecoder *) d, EtnToValue(&TreeNodeType, &out));

	free (e);
	etnBufferDecoderFree (d);

	return treeSame (in, &out);
}

UnitTest testTree = { __FILE__, _predicate };
