#ifndef __ETN_H__
#define __ETN_H__

#include <ethos/generic/slice.h>
#include <ethos/generic/types.h>
#include <ethos/generic/tree.h>
#include <ethos/dual/ethosTypes.h> // For HashSize.
#include <ethos/dual/status.h>

// This is the C library for encoding, decoding, and verifying ETN types.
// The library's interface is build around readers and writers, which can
// be overloaded to encode/decode/verify memory buffers, file descriptors,
// etc.
//
// EtnEncoder is a "writer" and can be considered a base class; things like
// EtnBufferEncoder specialize EtnEncoder as to what the writer writes to
// (in this case to encode to a buffer in memory). EtnDecoder and EtnVerifier 
// are similar.
//
// Libetn interacts with code generated using eg2source. Eg2source generates
// C code containing structures that describe types, and libetn consumes
// these definitions to perform encoding/decoding/verifying.
//
// There are quite a few simple examples of using libetn in the tests
// directory, and also code that benchmarks other systems such as XDR for
// comparison with libetn.  You should ignore the latter if you are trying 
// to learn libetn.  You will learn most of what you need to know by looking 
// at the tests. Here we provide a brief overview.
//
// Encoding:
//  First, create an encoder and provide it an output buffer:
//   EtnBufferEncoder *e = etnBufferEncoderNew(buf, length(buf));
//  Next, encode using etnEncode:
//   EtnLength size;
//   etnEncode((EtnEncoder *) e, EtnToValue(&Uint32Type, &in), &size);
//
// Decoding:
//  First, create a decoder and provide it an input buffer:
//   EtnBufferDecoder *d = etnBufferDecoderNew(buf, length(buf));
//  Next, decode using etnDecode:
//   etnDecode((EtnDecoder *) d, EtnToValue(&Uint32Type, &out));
//
// Verifying:
//  First, ensure you have registered the types you want to verify with libetn:
//   loadTypeGraph (&types, TestTypesPath);
//  Next, create a verifier and provide it an input buffer:
//   EtnBufferVerifier *v = etnBufferVerifierNew(types, buf, length(buf));
//  Finally, verify your data:
//   if (StatusOk != etnVerify ((EtnVerifier *) v, hash)) { ... }
//
// Resetting:
//  When you are finished (e.g.) encoding one buffer, you can reset
//  your (e.g.) encoder to use another. This is done as so:
//   etnBufferEncoderReset (e, newBuf, length(newBuf));

typedef uint32_t EtnLength;
typedef uint64_t EtnCallId;
typedef uint64_t EtnUnionId;
typedef uint8_t  EtnPtrEncoding;

// Types of pointers.
enum {
	EtnPtrNil = 0, // NULL.
	EtnPtrIndex,   // An address already seen and previously encoded/decoded.
	EtnPtrInline   // An address not yet seen.
};

// The parent verifier class.
typedef struct EtnVerifier_s {
	int (*consume) (struct EtnVerifier_s *v, EtnLength length); // Don't store to buffer like read;
	                                               // Just check length is available
	                                               // and move on.
	int (*read) (struct EtnVerifier_s *v, uint8_t *data, EtnLength length);
	EtnLength objCount;
	Slice typeGraph;
} EtnVerifier;

// Verifier subclass that verifies a provided memory range.
typedef struct EtnBufferVerifier_s {
	EtnVerifier verifier;
	uint8_t *dataCurrent;
	uint8_t *dataEnd;
} EtnBufferVerifier;

// The parent encoder class.
typedef struct EtnEncoder_s {
	int (*write)(struct EtnEncoder_s *e, uint8_t *data, EtnLength length);
	void (*flush)(struct EtnEncoder_s *e);
	void *topLevelPointer; // See comments in encoder.c.
	struct rbtree addrToIndex;
	EtnLength index;
} EtnEncoder;

// Encoder subclass that encodes to provided memory range.
typedef struct EtnBufferEncoder_s {
	EtnEncoder encoder;
	uint8_t *dataCurrent;
	uint8_t *dataEnd;
} EtnBufferEncoder;

// Encoder subclass that throws away encoded data;
// useful to determine what size of encoded data will be.
typedef struct EtnNullEncoder_s {
	EtnEncoder encoder;
} EtnNullEncoder;

// The parent decoder class.
typedef struct EtnDecoder_s {
	int (*read) (struct EtnDecoder_s *d, uint8_t *data, EtnLength length);
	void **indexToData;
	EtnLength indexToDataLength;
	EtnLength nextIndex;
} EtnDecoder;

// Decoder subclass that decodes from provided memory range.
typedef struct EtnBufferDecoder_s {
	EtnDecoder decoder;
	uint8_t *dataCurrent;
	uint8_t *dataEnd;
} EtnBufferDecoder;

// Decoder subclass (of BufferDecoder) that decodes from provided path.
typedef struct EtnPathDecoder_s {
	EtnBufferDecoder bufferDecoder;
	uint8_t *dataOriginal;
} EtnPathDecoder;

extern EtnType Int8Type;
extern EtnType Uint8Type;
extern EtnType Int16Type;
extern EtnType Uint16Type;
extern EtnType Int32Type;
extern EtnType Uint32Type;
extern EtnType Int64Type;
extern EtnType Uint64Type;
extern EtnType Float32Type;
extern EtnType Float64Type;
extern EtnType BoolType;
extern EtnType AnyType;
extern EtnType PtrEncodingType;
extern EtnType LengthType;
extern EtnType CallIdType;
extern EtnType UnionIdType;

extern EtnIndirectType StringType;
extern EtnIndirectType HashType;
extern EtnMethods NoMethods;

#define EtnToValue(type, x) ((EtnValue) { (EtnType *) type, (uint8_t *) (x) })
#define EtnToField(type, strct, field) ((EtnField) { (EtnType *) (type), __builtin_offsetof (strct, field) })

// Create a new buffer verifier.
EtnBufferVerifier *etnBufferVerifierNew(Slice graph, uint8_t *data, EtnLength length);

// Generic verifier reset; should only be called by subclasses.
void etnVerifierReset(EtnVerifier *v, int (*consume) (EtnVerifier *v, EtnLength length), int (*read) (EtnVerifier *v, uint8_t *data, EtnLength length));

// Reset buffer verifier and use a newly provided buffer.
void etnBufferVerifierReset(EtnBufferVerifier *v, uint8_t *data, EtnLength length);

// Create a new buffer encoder.
EtnBufferEncoder *etnBufferEncoderNew(uint8_t *data, EtnLength length);

// Create a new null encoder.
EtnNullEncoder *etnNullEncoderNew(void);

// Generic encoder reset; should only be called by subclasses.
void etnEncoderReset(EtnEncoder *e, int (*write) (EtnEncoder *e, uint8_t *data, EtnLength length), void (*flush) (EtnEncoder *e));

// Reset buffer encoder and use a newly provided buffer.
void etnBufferEncoderReset(EtnBufferEncoder *e, uint8_t *data, EtnLength length);

// Reset null encoder.
void etnNullEncoderReset(EtnNullEncoder *e);

// Perform an encode of EtnValue v;
// should pass as e a subclass that has been casted to Encoder *;
// returns status and sets length to length of encoded form.
Status etnEncode(EtnEncoder *e, EtnValue v, EtnLength *length);
Status etnEncodeWithoutFlush(EtnEncoder *e, EtnValue v, EtnLength *length);

// Create a new buffer decoder.
EtnBufferDecoder *etnBufferDecoderNew(uint8_t *data, EtnLength length);

// Reset buffer decoder and use a newly provided buffer.
void etnBufferDecoderReset(EtnBufferDecoder *d, uint8_t *data, EtnLength length);

// Free buffer decoder.
void etnBufferDecoderFree(EtnBufferDecoder *d);

// Create a new path decoder.
EtnPathDecoder *etnPathDecoderNew(char *path);

// Reset path decoder and use a newly provided path.
void etnPathDecoderReset(EtnPathDecoder *d, char *path);

// Generic decoder reset; should only be called by subclasses.
void etnDecoderReset(EtnDecoder *d, int (*read) (EtnDecoder *d, uint8_t *data, EtnLength length));

// Generic decoder free; should only be called by subclasses.
void etnDecoderFree(EtnDecoder *d);

// Perform a decode and place result in EtnValue v;
// should pass as d a subclass that has been casted to Decoder *.
Status etnDecode(EtnDecoder *d, EtnValue v);

// Create a new verifier.
EtnVerifier etnVerifierNew(Slice graph);

// Perform a verification.
Status etnVerify(EtnVerifier *v, uint8_t *hash);

// Given an encoder and decoder, create a new EtnRpcHost.
EtnRpcHost *etnRpcHostNew(EtnValue v, EtnEncoder *e, EtnDecoder *d);

// Get the encoder out of an EtnRpcHost.
EtnEncoder *etnRpcHostGetEncoder (EtnRpcHost *host);

Status etnRpcCall(EtnRpcHost *h, EtnCallId id, EtnValue args, EtnLength *length);
Status etnRpcHandle(EtnRpcHost *h);

// Register a type that may then be used as an any type.
int etnRegisterType(EtnType *t, uint8_t *h);

// Return the type associated with the given hash.
EtnType *etnTypeForHash(uint8_t *h);

// Return the hash associated with the given type.
Status etnHashForType(EtnType *t, uint8_t *h);

#endif
