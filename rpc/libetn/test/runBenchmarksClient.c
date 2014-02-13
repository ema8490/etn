#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <ethos/dual/xmalloc.h>
#include <ethos/generic/assert.h>

#include "unitTest.h"
#include "testGraph.h"
#include "treeFns.h"

enum {
        IterationCount = 100000,
	OutstandingRpcs = 20,
	EncodeBufferSize = 1024 * 10
};

// Benchmark allowing OutstandingRpcs outstanding RPCs.
// Benchmarking with one outstanding is essentially a measure of network
// latency. We use OutstandingRpcs to pipeline things, better stressing the
// RPC encoding/decoding itself.
#define BM(name, t, size, fn) \
{ \
	gettimeofday (&now, NULL); \
	for (i = 0; i < IterationCount + OutstandingRpcs; i++) { \
		uint64_t cid = 0; \
		if (i < IterationCount) fn; \
		if (i >= OutstandingRpcs) etnRpcHandle(h); \
	} \
	gettimeofday (&later, NULL); \
	report (name, now, later, size); \
}

void report (char *name, struct timeval start, struct timeval end, size_t size)
{
	printf ("%s: Ran %d iterations in %f seconds, total %"PRIu64" bytes\n",
	        name,
	        IterationCount,
	       (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0,
	        IterationCount * (uint64_t) size);
}

int main (int argc, char *argv[])
{
	ASSERT (31 == ((1 << TreeHeight) - 1));
	ASSERT (4096 == strlen (FarewellAddress));

	xmallocInit ();

	int i;
	EtnLength length;
	struct timeval now, later;
	uint8_t buf[EncodeBufferSize];

	if (argc != 2) {
		fprintf (stderr, "Usage: %s IPADDR\n", argv[0]);
		exit (EXIT_FAILURE);
	}
	
	EtnRpcHost *h = ipClientRpcHostNew (&RpcBenchmarkServer, argv[1], 1024, buf, EncodeBufferSize);

	bool t1 = 0;
	BM ("Boolean", t1, sizeof (bool), rpcBenchmarkSendBoolCall (h, cid, t1, &length));

	uint8_t t2 = 0;
	BM ("8-bit Integer", t2, sizeof (uint8_t), rpcBenchmarkSendUint8Call (h, cid, t2, &length));

	uint16_t t3 = 0;
	BM ("16-bit Integer", t3, sizeof (uint16_t), rpcBenchmarkSendUint16Call (h, cid, t3, &length));

	uint32_t t4 = 0;
	BM ("32-bit Integer", t4, sizeof (uint32_t), rpcBenchmarkSendUint32Call (h, cid, t4, &length));

	uint64_t t5 = 0;
	BM ("64-bit Integer", t5, sizeof (uint64_t), rpcBenchmarkSendUint64Call (h, cid, t5, &length));

	float t6 = 0;
	BM ("32-bit Float", t6, sizeof (float), rpcBenchmarkSendFloat32Call (h, cid, t6, &length));

	double t7 = 0;
	BM ("64-bit Float", t7, sizeof (double), rpcBenchmarkSendFloat64Call (h, cid, t7, &length));

	String *t8 = stringAllocateInitialize (FarewellAddress);
	BM ("4096-byte String", t8, strlen (FarewellAddress), rpcBenchmarkSendStringCall (h, cid, t8, &length));

	ByteArray t9;
	BM ("Byte Array", t9, sizeof (ByteArray), rpcBenchmarkSendByteArrayCall (h, cid, t9, &length));

	Uint32Array t10;
	BM ("32-bit Array", t10, sizeof (Uint32Array), rpcBenchmarkSendUint32ArrayCall (h, cid, t10, &length));

	CertificateHeader certificateHeader;
	IdentityCertificate certificate;
	certificate.header = certificateHeader;
	certificate.subject = stringAllocateInitialize (HelloWorld);
	BM ("Certificate",
	    certificate,
	    sizeof (IdentityCertificate) + sizeof (CertificateHeader) + strlen (HelloWorld),
	    rpcBenchmarkSendCertificateCall (h, cid, certificate, &length));

	Torture torture;
	torture.tuple = stringAllocateInitialize (HelloWorld);
	BM ("Torture",
	    torture,
	    sizeof (Torture) + strlen (HelloWorld),
	    rpcBenchmarkSendTortureCall (h, cid, torture, &length));

	TreeNode *node;
	node = treeBuild (TreeHeight);	
	BM ("32-node Tree", node, sizeof (TreeNode) * ((1 << TreeHeight) - 1), rpcBenchmarkSendTreeNodeCall (h, cid, *node, &length));

	exit (EXIT_SUCCESS);
}
