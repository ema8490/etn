#ifndef __UNITTEST_H__
#define __UNITTEST_H__

#include <ethos/generic/etn.h>
#include <ethos/generic/mixin.h>
#include <stdbool.h>

extern const char *HelloWorld;
extern const char *FarewellAddress;
extern const uint32_t TreeHeight;

extern char *TypeGraphPath;
extern char *TestTypesPath;

typedef struct UnitTest {
	char *name;
	bool (*predicate) (void);
} UnitTest;

typedef struct FdEncoder_s {
        EtnEncoder encoder;
	int fd;
} FdEncoder;

typedef struct FdDecoder_s {
        EtnDecoder decoder;
	int fd;
} FdDecoder;

typedef struct FdBufferedEncoder_s {
        FdEncoder fdEncoder;
	int bufSize;
	int used;
	uint8_t *buf;
} FdBufferedEncoder;

void loadTypeGraph (Slice *types, char *dir);

void runTest (UnitTest test);
void runBenchmarkE (char *name, EtnValue in, size_t size);
void runBenchmarkD (char *name, EtnValue in, EtnValue out, size_t size);
void runBenchmarkV (char *name, EtnValue in, uint8_t *hash, size_t size);

int fdDecoderRead (EtnDecoder *d, uint8_t *b, uint32_t s);
void fdDecoderReset(FdDecoder *d, int fd);
FdDecoder * fdDecoderNew(int fd);

int fdEncoderWrite (EtnEncoder *e, uint8_t *b, uint32_t s);
void fdEncoderFlush (EtnEncoder *e);
void fdEncoderReset(FdEncoder *e, int fd);
FdEncoder * fdEncoderNew(int fd);

int fdBufferedEncoderWrite (EtnEncoder *e, uint8_t *b, uint32_t s);
void fdBufferedEncoderFlush (EtnEncoder *e);
void fdBufferedEncoderReset(FdBufferedEncoder *e, int fd, uint8_t *buf, int bufSize);
FdBufferedEncoder * fdBufferedEncoderNew(int fd, uint8_t *buf, int bufSize);

EtnRpcHost *ipClientRpcHostNew (EtnType *type, char *hostname, uint16_t port, uint8_t *buf, int bufSize);
EtnRpcHost *ipServerRpcHostNew (EtnType *type, uint16_t port, uint8_t *buf, int bufSize);

#endif
