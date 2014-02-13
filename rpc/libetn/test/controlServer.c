#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

enum {
	Size = 120,
	iterationCount = 100000,
};

void runControlServer ()
{
	printf ("Server start.\n");

	int server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(server_sock < 0) {
		perror("Creating socket failed: ");
		exit(1);
	}

	// allow fast reuse of ports 
	int reuse_true = 1;
	setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &reuse_true, sizeof(reuse_true));

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1024);
	addr.sin_addr.s_addr = INADDR_ANY;

	int res = bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
	if(res < 0) {
		perror("Error binding to port");
		exit(1);
	}

	struct sockaddr_in remote_addr;
	unsigned int socklen = sizeof(remote_addr);

	res = listen(server_sock, 0);
	if(res < 0) {
		perror("Error listening for connection");
		exit(1);
	}

	int sock;
	sock = accept(server_sock, (struct sockaddr*)&remote_addr, &socklen);
	if(sock < 0) {
		perror("Error accepting connection");
		exit(1);
	}

	int i;
	char buf[Size];
	for (i = 0; i < iterationCount; i++) {
		read (sock, buf, Size);
	}

	printf ("Server done.\n");
}

int main (int argc, char *argv[])
{
	runControlServer ();

	exit (EXIT_SUCCESS);
}
