#include <ethos/generic/assert.h>
#include <rpc/xdr.h>
#include <stdio.h>
#include <stdlib.h>

#include "controlTypes.h"
#include "unitTest.h"
#include "xdrTreeFns.h"

enum {
        BufSize        = 1024 * 10,
	IterationCount = 10000000
};

// NOTE: Don't call xdr_free in BME: both in and out will be freed later by BMD.
#define BME(name, xdrIn, Type, d1) \
{ \
	int i; \
        Type in = d1; \
	struct timeval now, later; \
	gettimeofday (&now, NULL); \
	for (i = 0; i < IterationCount; i++) { \
		if (!xdr_##Type (&xdrIn, &in)) { \
			fprintf (stderr, "Encoding failure\n"); \
			exit (1); \
		} \
		xdr_setpos (&xdrIn, 0); \
	} \
	gettimeofday (&later, NULL); \
	printf ("%s: Ran %d iterations in %f seconds\n", \
		name, \
	        IterationCount, \
	       (later.tv_sec - now.tv_sec) + (later.tv_usec - now.tv_usec) / 1000000.0); \
}

#define BMD(name, xdrIn, xdrOut, Type, d1, d2) \
{ \
	int i; \
        Type in = d1, out = d2; \
	struct timeval now, later; \
	gettimeofday (&now, NULL); \
	if (!xdr_##Type (&xdrIn, &in)) { \
		fprintf (stderr, "Encoding failure\n"); \
		exit (1); \
	} \
	for (i = 0; i < IterationCount; i++) { \
		if (!xdr_##Type (&xdrOut, &out)) { \
			fprintf (stderr, "Decoding failure\n"); \
			exit (1); \
		} \
		xdr_setpos (&xdrIn, 0); \
		xdr_setpos (&xdrOut, 0); \
	} \
	gettimeofday (&later, NULL); \
	printf ("%s: Ran %d iterations in %f seconds\n", \
		name, \
	        IterationCount, \
	       (later.tv_sec - now.tv_sec) + (later.tv_usec - now.tv_usec) / 1000000.0); \
        xdr_free ((xdrproc_t) xdr_##Type, (char *) &in); \
        xdr_free ((xdrproc_t) xdr_##Type, (char *) &out); \
}

void benchmarkEncodeUint32Array (XDR xdrIn)
{
	int i;
        Uint32Array in;
	struct timeval now, later;
	gettimeofday (&now, NULL);
	for (i = 0; i < IterationCount; i++) {
		if (!xdr_Uint32Array (&xdrIn, in)) {
			fprintf (stderr, "Encoding failure\n");
			exit (1);
		}
		xdr_setpos (&xdrIn, 0);
	}
	gettimeofday (&later, NULL);
	printf ("32-bit Array: Ran %d iterations in %f seconds\n",
	        IterationCount,
	       (later.tv_sec - now.tv_sec) + (later.tv_usec - now.tv_usec) / 1000000.0);
        xdr_free ((xdrproc_t) xdr_Uint32Array, (char *) in);
}

void benchmarkDecodeUint32Array (XDR xdrIn, XDR xdrOut)
{
	int i;
        Uint32Array in, out;
	struct timeval now, later;
	gettimeofday (&now, NULL);
	if (!xdr_Uint32Array (&xdrIn, in)) {
		fprintf (stderr, "Encoding failure\n");
		exit (1);
	}
	for (i = 0; i < IterationCount; i++) {
		if (!xdr_Uint32Array (&xdrOut, out)) {
			fprintf (stderr, "Deconding failure\n");
			exit (1);
		}
		xdr_setpos (&xdrIn, 0);
		xdr_setpos (&xdrOut, 0);
	}
	gettimeofday (&later, NULL);
	printf ("32-bit Array: Ran %d iterations in %f seconds\n",
	        IterationCount,
	       (later.tv_sec - now.tv_sec) + (later.tv_usec - now.tv_usec) / 1000000.0);
        xdr_free ((xdrproc_t) xdr_Uint32Array, (char *) in);
        xdr_free ((xdrproc_t) xdr_Uint32Array, (char *) out);
}

int main (int argc, char *argv[])
{
	ASSERT (31 == ((1 << TreeHeight) - 1));
	ASSERT (4096 == strlen (FarewellAddress));

        XDR xdrIn, xdrOut;
        char buf[BufSize];

	CertificateHeader header;
        IdentityCertificate inCert, outCert;
	inCert.header = header;
	inCert.subject = strdup (HelloWorld);
	outCert.subject = NULL;

        Torture inTorture, outTorture;
	inTorture.p = malloc (sizeof (uint32_t));
	inTorture.t = strdup (HelloWorld);
	outTorture.p = NULL;
	outTorture.t = NULL;

	TreeNode *inTreeNode = treeBuild (TreeHeight), outTreeNode;

        xdrmem_create (&xdrIn, buf, BufSize, XDR_ENCODE);
        xdrmem_create (&xdrOut, buf, BufSize, XDR_DECODE);

	printf ("\n");
	printf ("Encode benchmarks:\n");

	BME ("32-bit Integer", xdrIn, Uint32, 0);
	BME ("64-bit Integer", xdrIn, Uint64, 0);
	BME ("32-bit Float", xdrIn, Float32, 0);
	BME ("64-bit Float", xdrIn, Float64, 0);
	BME ("Pointer", xdrIn, UintPtr, malloc (1));
	BME ("4096-byte String", xdrIn, String, strdup(FarewellAddress));
	benchmarkEncodeUint32Array (xdrIn);
	BME ("Certificate", xdrIn, IdentityCertificate, inCert);
	BME ("Torture", xdrIn, Torture, inTorture);
	BME ("32-node Tree", xdrIn, TreeNode, *inTreeNode);

	printf ("\n");
	printf ("Decode benchmarks:\n");

	BMD ("32-bit Integer", xdrIn, xdrOut, Uint32, 0, 0);
	BMD ("64-bit Integer", xdrIn, xdrOut, Uint64, 0, 0);
	BMD ("32-bit Float", xdrIn, xdrOut, Float32, 0, 0);
	BMD ("64-bit Float", xdrIn, xdrOut, Float64, 0, 0);
	BMD ("Pointer", xdrIn, xdrOut, UintPtr, malloc (1), NULL);
	BMD ("4096-byte String", xdrIn, xdrOut, String, strdup(FarewellAddress), NULL);
	benchmarkDecodeUint32Array (xdrIn, xdrOut);
	BMD ("Certificate", xdrIn, xdrOut, IdentityCertificate, inCert, outCert);
	BMD ("Torture", xdrIn, xdrOut, Torture, inTorture, outTorture);
	BMD ("32-node Tree", xdrIn, xdrOut, TreeNode, *inTreeNode, outTreeNode);

        xdr_destroy (&xdrIn);
        xdr_destroy (&xdrOut);

	exit (EXIT_SUCCESS);
}
