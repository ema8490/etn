#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ethos/dual/xmalloc.h>
#include <ethos/generic/etn.h>

#include "rpcDemoCommon.h"
#include "testGraph.h"

#define arrayLen(x) (sizeof(x)/sizeof(x[0]))

int main(int argc, char *argv[]) {
	xmallocInit ();

	int sock;
	EtnLength length;
	uint64_t cid = 0;
	char *ip = "127.0.0.1";
	uint16_t port = 12345;
	struct sockaddr_in addr = {.sin_family=AF_INET};

	if (argc > 1) {
		char *cln = strchr(argv[1], ':');
		if (!cln) {
			exit(EXIT_FAILURE);
		}
		*cln = '\0';
		ip = argv[1];
		port = (uint16_t)strtol(&cln[1], 0, 0);
	}

	addr.sin_port = htons(port);
	inet_aton(ip, &addr.sin_addr);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
		perror ("Error opening socket");
		exit(EXIT_FAILURE);
	}

	FdEncoder *e = fdEncoderNew(sock);
	FdDecoder *d = fdDecoderNew(sock);

	EtnRpcHost *h = etnRpcHostNew(EtnToValue(&RpcTestServer, 0), (EtnEncoder *) e, (EtnDecoder *) d);

	rpcTestHelloCall(h, cid++, &length);
	// Do not wait for response; no return value.

	String *msg = stringAllocateInitialize ("Hello, parameterized world!");
	rpcTestSprintfCall(h, cid++, msg, &length);
	etnRpcHandle(h);           // Wait for response.

	String *one = stringAllocateInitialize ("One");
	String *two = stringAllocateInitialize ("Two");
	String *value = stringAllocateInitialize ("Value");
	rpcTestTwoArgsOneReturnCall(h, cid++, one, two, &length);
	etnRpcHandle(h);           // Wait for response.

	rpcTestOneArgTwoReturnsCall(h, cid++, one, &length);
	etnRpcHandle(h);           // Wait for response.

	ByteArray array;
	strcpy ((char *) array, "foo");
	rpcTestArrayArgCall(h, cid++, array, &length);
	etnRpcHandle(h);

	rpcTestTupleArgCall(h, cid++, 0, value, &length);
	etnRpcHandle(h);

	// Try with an ampty slice.
	String *empty = stringEmpty ();
	rpcTestTupleArgCall(h, cid++, 0, empty, &length);
	etnRpcHandle(h);

	printf ("passed\n");

	exit(EXIT_SUCCESS);
}
