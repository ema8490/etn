#include <stdlib.h>
#include <string.h>

#include "unitTest.h"
#include "testGraph.h"

static bool
_predicate (void)
{
	// Note: you may find the hash for the RpcTest interface using egPrint.
	uint8_t hash[] = { 0xfd, 0x34, 0x86, 0x48, 0x92, 0x90, 0x95, 0x91, 0xa5, 0x88, 0x18, 0x85, 0x83, 0xf9, 0x5b, 0xfa, 0x9a, 0x6c, 0xe7, 0x6, 0xd2, 0xa, 0xb3, 0x3d, 0xc4, 0x5e, 0xc6, 0x9d, 0x17, 0xb3, 0x44, 0x62, 0xc7, 0xf4, 0x52, 0xef, 0x5d, 0xd6, 0xa, 0x7b, 0xed, 0xb7, 0x99, 0xa7, 0x9a, 0xf6, 0x1f, 0x84, 0xd7, 0xe3, 0xcb, 0x8c, 0xa7, 0xe7, 0x62, 0xc1, 0x7, 0xff, 0xdd, 0xd6, 0x68, 0x19, 0xa2, 0xcf };

	Slice types;
	loadTypeGraph (&types, TestTypesPath);

	uint8_t buf[1024];
	EtnBufferEncoder *e = etnBufferEncoderNew(buf, sizeof(buf));
	EtnBufferDecoder *d = NULL;
	EtnRpcHost *h = etnRpcHostNew(EtnToValue(&RpcTestServer, 0), (EtnEncoder *) e, (EtnDecoder *) d);

	EtnBufferVerifier *v = etnBufferVerifierNew(types, buf, sizeof(EtnCallId) + sizeof(uint64_t));
	EtnLength length;
	rpcTestHelloCall(h, 0, &length);
	if (StatusOk != etnVerify ((EtnVerifier *) v, hash)) {
		return false;
	}

	etnBufferEncoderReset  (e, buf, sizeof(buf));
	etnBufferVerifierReset (v, buf, sizeof(EtnCallId) + sizeof(uint64_t) + sizeof (EtnLength) + strlen (HelloWorld));
	rpcTestSprintfCall(h, 0, stringAllocateInitialize(HelloWorld), &length);
	if (StatusOk != etnVerify ((EtnVerifier *) v, hash)) {
		return false;
	}

	// FIXME: Does not test negative cases.

	free (e);
	free (d);
	free (v);

	return true;
}

UnitTest testVerifyInterface = { __FILE__, _predicate };
