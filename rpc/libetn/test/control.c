#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

enum {
	Size = 120,
	IterationCount = 100000
};

unsigned char in[Size * IterationCount], out[Size * IterationCount];

int main (int argc, char *argv[])
{
	CertificateHeader certificateIn, certificateOut;

	certificateIn.size    = 0xdeadbeef;
	certificateIn.version = 0x8badf00d;
	memset (certificateIn.validFrom, 0x0f, sizeof (certificateIn.validFrom));
        memset (certificateIn.validTo, 0x0f, sizeof (certificateIn.validTo));
        memset (certificateIn.typeHash, 0x0f, sizeof (certificateIn.typeHash));
        memset (certificateIn.publicSignatureKey, 0x0f, sizeof (certificateIn.publicSignatureKey));
        memset (certificateIn.revocationServer, 0x0f, sizeof (certificateIn.revocationServer));
        memset (certificateIn.signature, 0x0f, sizeof (certificateIn.signature));

	memcpy(&certificateOut, &certificateIn, sizeof(CertificateHeader));

	int i;
	// Copy certificate to compare to benchmark.c.
	for (i = 0; i < IterationCount; i++) {
		memcpy(out + Size * i, in + Size * i, Size);
	}

	printf ("Ran %d iterations:\n", IterationCount);
	printf ("0x%x\n", certificateOut.size);
	printf ("0x%x\n", certificateOut.version);

	exit (EXIT_SUCCESS);
}
