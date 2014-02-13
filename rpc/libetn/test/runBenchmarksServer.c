#include <stdio.h>
#include <stdlib.h>
#include <ethos/dual/xmalloc.h>

#include "unitTest.h"
#include "testGraph.h"

enum {
	EncodeBufferSize = 1024 * 10
};

int main (int argc, char *argv[])
{
	xmallocInit ();

	uint8_t buf[EncodeBufferSize];

	EtnRpcHost *h = ipServerRpcHostNew (&RpcBenchmarkServer, 1024, buf, EncodeBufferSize);

	while (true) {
		etnRpcHandle(h);
	}

	exit (EXIT_SUCCESS);
}
