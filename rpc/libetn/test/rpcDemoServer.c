#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdint.h>
#include <errno.h>
#include <ethos/dual/xmalloc.h>
#include <ethos/generic/etn.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "rpcDemoCommon.h"
#include "testGraph.h"

uint8_t hash[] = {0x27, 0x57, 0xcb, 0x3c, 0xaf, 0xc3, 0x9a, 0xf4, 0x51, 0xab, 0xb2, 0x69, 0x7b, 0xe7, 0x9b, 0x4a, 0xb6, 0x1d, 0x63, 0xd7, 0x4d, 0x85, 0xb0, 0x41, 0x86, 0x29, 0xde, 0x8c, 0x26, 0x81, 0x1b, 0x52, 0x9f, 0x3f, 0x37, 0x80, 0xd0, 0x15, 0x0, 0x63, 0xff, 0x55, 0xa2, 0xbe, 0xee, 0x74, 0xc4, 0xec, 0x10, 0x2a, 0x2a, 0x27, 0x31, 0xa1, 0xf1, 0xf7, 0xf1, 0xd, 0x47, 0x3a, 0xd1, 0x8a, 0x6a, 0x87};

int main(int argc, char *argv[]) {
	xmallocInit ();

	int rv, lsock, sock;

	struct sockaddr_in addr = {.sin_family=AF_INET, .sin_addr.s_addr = INADDR_ANY, .sin_port=htons(12345)};
	lsock = socket(AF_INET, SOCK_STREAM, 0);

	/* Allow fast reuse of ports; without this, Linux will not allow you to
	 * reopen a port for some time after the process exits.
	 */
        int reuse_true = 1;
	setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &reuse_true, sizeof(reuse_true));

	if ((rv = bind(lsock, (struct sockaddr*) &addr, sizeof(addr))) < 0) {
		perror("error");
		exit(rv);
	}

	if ((rv = listen(lsock, 50)) < 0) {
		perror("error");
		exit(rv);
	}

	if ((sock = accept(lsock, 0, 0)) < 0) {
		perror("error");
		exit(sock);
	}

	etnRegisterType((EtnType *)&StringType, hash);

	FdEncoder *e = fdEncoderNew(sock);
	FdDecoder *d = fdDecoderNew(sock);

	EtnRpcHost *h = etnRpcHostNew(EtnToValue(&RpcTestServer, 0), (EtnEncoder *) e, (EtnDecoder *) d);

	etnRpcHandle(h);
	etnRpcHandle(h);
	etnRpcHandle(h);
	etnRpcHandle(h);
	etnRpcHandle(h);
	etnRpcHandle(h);
	etnRpcHandle(h);

	printf ("passed\n");

	close(sock);
	close(lsock);

	exit(EXIT_SUCCESS);
}
