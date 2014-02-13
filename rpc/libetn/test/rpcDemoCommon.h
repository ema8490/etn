#include <ethos/generic/etn.h>

// Encoder subclass that encodes to provided memory range.
typedef struct FdEncoder_s {
	EtnEncoder encoder;
	int fd;
} FdEncoder;

// Decoder subclass that decodes to provided memory range.
typedef struct FdDecoder_s {
	EtnDecoder decoder;
	int fd;
} FdDecoder;

int _read(EtnDecoder *d, uint8_t *data, uint32_t len);

int _write(EtnEncoder *e, uint8_t *data, uint32_t len);

void _flush(EtnEncoder *e);

// Reset fd encoder and use a newly provided fd.
void fdEncoderReset(FdEncoder *e, int fd);

// Create a new buffer encoder.
FdEncoder *fdEncoderNew(int fd);

// Reset fd decoder and use a newly provided fd.
void fdDecoderReset(FdDecoder *d, int fd);

// Create a new buffer encoder.
FdDecoder *fdDecoderNew(int fd);
