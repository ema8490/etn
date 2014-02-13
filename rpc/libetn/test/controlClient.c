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

static void
runControlClient (char *host)
{
	printf ("Client start.\n");

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1024);

	struct hostent *hp = gethostbyname (host);
	if (hp == NULL || hp->h_addr_list[0] == NULL) {
		perror ("Error resolving given host:");
		exit (EXIT_FAILURE);
	}

	memcpy (&(addr.sin_addr.s_addr), hp->h_addr_list[0], hp->h_length);

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0) {
		perror("Creating socket failed: ");
		exit(1);
	}

	int res = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
	if(res < 0) {
		perror("Error connecting");
		exit (EXIT_FAILURE);
	}

	int i;
	char buf[Size];
	for (i = 0; i < iterationCount; i++) {
		write (sock, buf, Size);
	}

	printf ("Client done.\n");
	
}

int main (int argc, char *argv[])
{
	if (argc != 2) {
		fprintf (stderr, "Usage: %s HOST\n", argv[0]);
		exit (EXIT_FAILURE);
	}

	runControlClient (argv[1]);

	exit (EXIT_SUCCESS);
}
