#ifndef __TYPES_H__
#define __TYPES_H__

#include <ethos/dual/ethosTypes.h> // For Slice.

// FIXME: How to keep synchronized with Go's types.go?
//        encoder.c and decoder.c tables?

extern int etnScalarSizeMap[];

typedef enum {
    // NOTE: The order of the scalars here must
    // match etnScalarSizeMap in types.c.
    EtnKindInt8 = 0,
    EtnKindUint8,
    EtnKindBool,
    // NOTE: Byte types *must* remain first
    // and *must* be followed by EtnKindInt16. This
    // is used by the encoder and decoder
    // to cheat when processing vector types.
    EtnKindInt16,
    EtnKindUint16,
    EtnKindInt32,
    EtnKindUint32,
    EtnKindInt64,
    EtnKindUint64,
    EtnKindFloat32,
    EtnKindFloat64,
    // NOTE: Scalar types *must* remain first
    // and *must* be followed by EtnKindString. This
    // is used by the verifier to cheat when
    // when processing vector types.
    EtnKindString,
    EtnKindAny,
    EtnKindUnion,
    EtnKindType,
    EtnKindTuple,
    EtnKindArray,
    EtnKindDict,
    EtnKindStruct,
    EtnKindIface,
    EtnKindFunc,
    EtnKindList,
    EtnKindPtr,
    EtnKindSptr,
    // NOTE: EtnKindInvalid *must* remain highest value.
    EtnKindInvalid,
    // Aliases:
    EtnKindPtrEncoding = EtnKindUint8,
    EtnKindLength      = EtnKindUint32,
    EtnKindCallId      = EtnKindUint64,
    EtnKindUnionId     = EtnKindUint64,
} EtnKind;


#define etnIsByte(kind) ((kind) < EtnKindInt16)
#define etnIsScalar(kind) ((kind) < EtnKindString)

typedef struct EtnRpcHost EtnRpcHost;    // Defined in erpc.c.

typedef struct {
    struct EtnMethodType *list;
    unsigned long num;
} EtnMethods;

typedef struct EtnType {
    EtnKind kind;
    uint32_t size;
    EtnMethods methods;
} EtnType;

typedef EtnType EtnInterfaceType;

// For arrays, size is len * size of type.
// For pointers, size is sizeof (void *).
// For Tuples, size is sizeof (Tuple).
typedef struct {
    EtnType type;
    EtnType *elem;
} EtnIndirectType;

typedef struct {
    EtnType type;
    EtnType *elem;
    unsigned long len;
} EtnCompositeType;

typedef struct {
    EtnType *elem;
    unsigned long offset;
} EtnField;

typedef struct {
    EtnType type;
    EtnField *fields;
    unsigned long len;
} EtnStructType;

typedef struct {
    EtnType type;
    EtnField *fields;
    unsigned long len;
} EtnUnionType;

typedef void (*EtnMethod) (EtnRpcHost *, void*);

typedef struct EtnMethodType {
    EtnType type;
    EtnMethod call;
    EtnStructType *params;
} EtnMethodType;

typedef struct EtnDict {
    unsigned long size;
    void *keys;
    void *values;
} EtnDict;

typedef struct EtnValue {
    EtnType *type;
    void *data;
} EtnValue;

#endif
