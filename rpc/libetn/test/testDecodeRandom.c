#include <stdlib.h>
#include <time.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	int i;
	EtnBufferDecoder *d = etnBufferDecoderNew(NULL, 0);

	// Let's feed this thing some random data.
	// I suppose there is a chance that we could
	// produce a valid encoding, but it's unlikely.
	srand (time (NULL));
	for (i = 0; i < 10000; i++) {
		Torture out;
		int n = 1024 + rand () % 1024;
		uint8_t *randomData = malloc (n);
		int j;

		for (j = 0; j < n; j++) {
			randomData[j] = (uint8_t) rand ();
		}

		etnBufferDecoderReset(d, randomData, n);
		if (StatusOk == etnDecode ((EtnDecoder *) d, EtnToValue(&TortureType, &out))) {
			return false;
		}

		free (randomData);
	}

	free (d);

	return true;
}

UnitTest testDecodeRandom = { __FILE__, _predicate };
