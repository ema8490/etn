#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <ethos/generic/assert.h>

#include "unitTest.h"
#include "testGraph.h"

#define BUF_USED  (buf.writePtr - buf.buf)
#define BUF_AVAIL (buf.writePtr - buf.readPtr)

const char *HelloWorld = "Hello, world!";

// Repeated to match size of 1024-byte uint32 array.
const char *FarewellAddress = 
	"Friends and Fellow-Citizens:"
        "The period for a new election of a citizen to "
        "administer the executive government of the "
        "United States being not far distant, and the "
        "time actually arrived when your thoughts must "
        "be employed in designating the person who is "
        "to be clothed with that important trust, "
        "it appears to me proper, especially as it may "
        "conduce to a more distinct expression of the "
        "public voice, that I should now apprise you "
        "of the resolution I have formed, to decline "
        "being considered among the number of those "
        "out of whom a choice is to be made."
	"Friends and Fellow-Citizens:"
        "The period for a new election of a citizen to "
        "administer the executive government of the "
        "United States being not far distant, and the "
        "time actually arrived when your thoughts must "
        "be employed in designating the person who is "
        "to be clothed with that important trust, "
        "it appears to me proper, especially as it may "
        "conduce to a more distinct expression of the "
        "public voice, that I should now apprise you "
        "of the resolution I have formed, to decline "
        "being considered among the number of those "
        "out of whom a choice is to be made."
	"Friends and Fellow-Citizens:"
        "The period for a new election of a citizen to "
        "administer the executive government of the "
        "United States being not far distant, and the "
        "time actually arrived when your thoughts must "
        "be employed in designating the person who is "
        "to be clothed with that important trust, "
        "it appears to me proper, especially as it may "
        "conduce to a more distinct expression of the "
        "public voice, that I should now apprise you "
        "of the resolution I have formed, to decline "
        "being considered among the number of those "
        "out of whom a choice is to be made."
	"Friends and Fellow-Citizens:"
        "The period for a new election of a citizen to "
        "administer the executive government of the "
        "United States being not far distant, and the "
        "time actually arrived when your thoughts must "
        "be employed in designating the person who is "
        "to be clothed with that important trust, "
        "it appears to me proper, especially as it may "
        "conduce to a more distinct expression of the "
        "public voice, that I should now apprise you "
        "of the resolution I have formed, to decline "
        "being considered among the number of those "
        "out of whom a choice is to be made."
	"Friends and Fellow-Citizens:"
        "The period for a new election of a citizen to "
        "administer the executive government of the "
        "United States being not far distant, and the "
        "time actually arrived when your thoughts must "
        "be employed in designating the person who is "
        "to be clothed with that important trust, "
        "it appears to me proper, especially as it may "
        "conduce to a more distinct expression of the "
        "public voice, that I should now apprise you "
        "of the resolution I have formed, to decline "
        "being considered among the number of those "
        "out of whom a choice is to be made."
	"Friends and Fellow-Citizens:"
        "The period for a new election of a citizen to "
        "administer the executive government of the "
        "United States being not far distant, and the "
        "time actually arrived when your thoughts must "
        "be employed in designating the person who is "
        "to be clothed with that important trust, "
        "it appears to me proper, especially as it may "
        "conduce to a more distinct expression of the "
        "public voice, that I should now apprise you "
        "of the resolution I have formed, to decline "
        "being considered among the number of those "
        "out of whom a choice is to be made."
	"Friends and Fellow-Citizens:"
        "The period for a new election of a citizen to "
        "administer the executive government of the "
        "United States being not far distant, and the "
        "time actually arrived when your thoughts must "
        "be employed in designating the person who is "
        "to be clothed with that important trust, "
        "it appears to me proper, especially as it may "
        "conduce to a more distinct expression of the "
        "public voice, that I should now apprise you "
        "of the resolution I have formed, to decline "
        "being considered among the number of those "
        "out of whom a choice is to be made."
	"Friends and Fellow-Citizens:"
        "The period for a new election of a citizen to "
        "administer the executive government of the "
        "United States being not far distant, and the "
        "time actually arrived when your thoughts must "
        "be employed in designating the "
;

const uint32_t TreeHeight = 5;

enum {
	IterationCount      = 10000000,
	BenchmarkBufferSize = 1024 * 10
};

char *TypeGraphPath = "../lib/ethos/typeGraph";
char *TestTypesPath = "testGraph";

void loadTypeGraph (Slice *types, char *dirName)
{
	DIR *dir = opendir(dirName);
        ASSERT (dir);

        // Get number of files.
        int nodeCount = 0;
        struct dirent *dirent;
        while ((dirent = readdir (dir))) {
		if (! strcmp (dirent->d_name, ".")
		 || ! strcmp (dirent->d_name, "..")) {
			continue;
		}
		nodeCount++;
        }
        
        types->array = malloc (nodeCount * sizeof (TypeNode));
	types->typesize = sizeof (TypeNode);
	types->len = nodeCount;
	types->cap = nodeCount;
        
        rewinddir (dir);
        while ((dirent = readdir (dir))) {
		if (! strcmp (dirent->d_name, ".")
		 || ! strcmp (dirent->d_name, "..")) {
			continue;
		}
		char path[PATH_MAX + 1];
		sprintf (path, "%s/%s", dirName, dirent->d_name);
		Slice type;
		Status status = mixinReadVar (&type, path);
		REQUIRE (StatusOk == status);
		EtnBufferDecoder *d = etnBufferDecoderNew(type.array, type.len);
		etnDecode((EtnDecoder *) d, EtnToValue(&TypeNodeType, &((TypeNode *) types->array)[--nodeCount]));
        }	
}

void runTest (UnitTest test)
{
	printf (test.predicate () ? "    ok" : "FAILED");
	printf (": test %s\n", test.name);
}

void runBenchmarkE (char *name, EtnValue in, size_t size)
{
        int i;
	uint8_t buffer[BenchmarkBufferSize];
	struct timeval now, later;

	EtnBufferEncoder *e = etnBufferEncoderNew(NULL, 0);

	gettimeofday (&now, NULL);
	for (i = 0; i < IterationCount - 1; i++) {
		EtnLength length;
		etnBufferEncoderReset(e, buffer, sizeof(buffer));
		etnEncode((EtnEncoder *) e, in, &length);
	}
	gettimeofday (&later, NULL);

	printf ("%s: Ran %d iterations in %f seconds, total %lu bytes\n",
		name,
		IterationCount,
		(later.tv_sec - now.tv_sec) + (later.tv_usec - now.tv_usec) / 1000000.0,
		IterationCount * (unsigned long) size);
}

void runBenchmarkD (char *name, EtnValue in, EtnValue out, size_t size)
{
        int i;
	uint8_t buffer[BenchmarkBufferSize];
	struct timeval now, later;

	EtnBufferEncoder *e = etnBufferEncoderNew(buffer, sizeof(buffer));
	EtnBufferDecoder *d = etnBufferDecoderNew(NULL, 0);

	uint32_t encodedSize;
	etnEncode((EtnEncoder *) e, in, &encodedSize);

	gettimeofday (&now, NULL);
	for (i = 0; i < IterationCount - 1; i++) {
		etnBufferDecoderReset(d, buffer, encodedSize);
		etnDecode((EtnDecoder *) d, out);

		switch (out.type->kind) {
			case EtnKindString:
				REFCHECK (charXtype, *((String **) out.data));
				stringUnhook ((String **) out.data);
				break;
			case EtnKindTuple:
				REFCHECK (ucharXtype, *((Ref **) out.data));
				refUnhook ((Ref **) out.data);
				break;
			default:
				;
		}
	}
	gettimeofday (&later, NULL);

	printf ("%s: Ran %d iterations in %f seconds, total %lu bytes\n",
		name,
		IterationCount,
		(later.tv_sec - now.tv_sec) + (later.tv_usec - now.tv_usec) / 1000000.0,
		IterationCount * (unsigned long) size);
}

void runBenchmarkV (char *name, EtnValue in, uint8_t *hash, size_t size)
{
        int i;
	Slice types;
	uint8_t buffer[BenchmarkBufferSize];
	struct timeval now, later;

	loadTypeGraph (&types, TestTypesPath);

	EtnBufferEncoder *e = etnBufferEncoderNew(buffer, sizeof (buffer));
	EtnBufferVerifier *v = etnBufferVerifierNew(types, NULL, 0);

	uint32_t encodedSize;
	etnEncode((EtnEncoder *) e, in, &encodedSize);
	Status status;

	gettimeofday (&now, NULL);
	for (i = 0; i < IterationCount - 1; i++) {
		etnBufferVerifierReset(v, buffer, encodedSize);
		status = etnVerify((EtnVerifier *) v, hash);
	}
	gettimeofday (&later, NULL);

	ASSERT (StatusOk == status);

	printf ("%s: Ran %d iterations in %f seconds, total %lu bytes\n",
		name,
		IterationCount,
		(later.tv_sec - now.tv_sec) + (later.tv_usec - now.tv_usec) / 1000000.0,
		IterationCount * (unsigned long) size);
}

int 
fdEncoderWrite (EtnEncoder *e, uint8_t *b, uint32_t s)
{
	return write (((FdEncoder *) e)->fd, b, s);
}

void
fdEncoderFlush (EtnEncoder *e)
{
	fsync(((FdEncoder *) e)->fd);
}

void
fdEncoderReset(FdEncoder *e, int fd)
{
	ASSERT(e);
	etnEncoderReset ((EtnEncoder *) e, fdEncoderWrite, fdEncoderFlush);
	e->fd = fd;
}

// create am encoder
FdEncoder *
fdEncoderNew(int fd)
{
	FdEncoder *d = malloc (sizeof (FdEncoder));
	ASSERT(d);
	fdEncoderReset(d, fd);
	return d;
}

int 
fdBufferedEncoderWrite (EtnEncoder *_e, uint8_t *b, uint32_t s)
{
	int fnval = 0;
	FdBufferedEncoder *e = (FdBufferedEncoder *) _e;
	if (s > e->bufSize - e->used) {
		fnval += write (((FdEncoder *) e)->fd, e->buf, e->used);
		fnval += write (((FdEncoder *) e)->fd, b, s);
		e->used = 0;
	} else {
		memcpy (e->buf + e->used, b, s);
		fnval = s;	
		e->used += s;
	}

	return fnval;
}

void
fdBufferedEncoderFlush (EtnEncoder *_e)
{
	FdBufferedEncoder *e = (FdBufferedEncoder *) _e;
	write (((FdEncoder *) e)->fd, e->buf, e->used);
	e->used = 0;
	fsync(((FdEncoder *) e)->fd);
}

void
fdBufferedEncoderReset(FdBufferedEncoder *_e, int fd, uint8_t *buf, int bufSize)
{
	ASSERT(_e);
	FdBufferedEncoder *e = (FdBufferedEncoder *) _e;
	etnEncoderReset ((EtnEncoder *) e, fdBufferedEncoderWrite, fdBufferedEncoderFlush);
	((FdEncoder *) e)->fd = fd;
	e->buf = buf;
	e->bufSize = bufSize;
	e->used = 0;
}

// create am encoder
FdBufferedEncoder *
fdBufferedEncoderNew(int fd, uint8_t *buf, int bufSize)
{
	FdBufferedEncoder *d = malloc (sizeof (FdBufferedEncoder));
	ASSERT(d);
	fdBufferedEncoderReset(d, fd, buf, bufSize);
	return d;
}

int 
fdDecoderRead (EtnDecoder *d, uint8_t *b, uint32_t s)
{
	int fnval = 0;
	while (fnval < s) {
		fnval += read (((FdDecoder *) d)->fd, b + fnval, s - fnval);
	}
	return fnval;
}

void
fdDecoderReset(FdDecoder *d, int fd)
{
	ASSERT(d);
	etnDecoderReset ((EtnDecoder *) d, fdDecoderRead);
	d->fd = fd;
}

// create a decoder
FdDecoder *
fdDecoderNew(int fd)
{
	FdDecoder *d = malloc (sizeof (FdDecoder));
	ASSERT(d);
	memset (d, 0x00, sizeof (FdDecoder)); // This must be done before Reset.
	fdDecoderReset(d, fd);
	return d;
}

EtnRpcHost *ipClientRpcHostNew (EtnType *type, char *ipAddr, uint16_t port, uint8_t *buf, int bufSize)
{
	int sock;
        struct sockaddr_in addr = {.sin_family=AF_INET};

        addr.sin_port = htons(1024);
        if (0 == inet_aton(ipAddr, &addr.sin_addr)) {
		fprintf (stderr, "Bad IP address: %s\n", ipAddr);
		exit (EXIT_FAILURE);
	}

	printf ("Connecting to %s...\n", inet_ntoa(addr.sin_addr));

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
                perror ("Error opening socket");
                exit (EXIT_FAILURE);
        }

        FdBufferedEncoder *e = fdBufferedEncoderNew(sock, buf, bufSize);
        FdDecoder *d = fdDecoderNew(sock);	

	return etnRpcHostNew(EtnToValue(type, 0), (EtnEncoder *) e, (EtnDecoder *) d);
}

EtnRpcHost *ipServerRpcHostNew (EtnType *type, uint16_t port, uint8_t *buf, int bufSize)
{
	int rv, lsock, sock;

	struct sockaddr_in addr = {.sin_family=AF_INET, .sin_addr.s_addr = INADDR_ANY, .sin_port=htons(port)};
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

        FdBufferedEncoder *e = fdBufferedEncoderNew(sock, buf, bufSize);
	FdDecoder *d = fdDecoderNew(sock);

	return etnRpcHostNew(EtnToValue(type, 0), (EtnEncoder *) e, (EtnDecoder *) d);
}
