#include <ethos/generic/assert.h>
#include <ethos/generic/debug.h>
#include <ethos/generic/etn.h>
#include <ethos/generic/mixin.h>
#include <ethos/dual/string.h>
#include <ethos/typeGraph.h>
#include <endian.h>
#include <stdint.h>

#if __BYTE_ORDER == __BIG_ENDIAN
#error Big endian not presently supported; need to byteswap scalars (and see etnIsScalar), etc.
#endif

static Status _verifyScalar        (EtnVerifier *v, TypeNode *n);
static Status _verifyString        (EtnVerifier *v, TypeNode *n);
static Status _verifyAny           (EtnVerifier *v, TypeNode *n);
static Status _verifyUnion         (EtnVerifier *v, TypeNode *n);
static Status _verifyType          (EtnVerifier *v, TypeNode *n);
static Status _verifyTuple         (EtnVerifier *v, TypeNode *n);
static Status _verifyStruct        (EtnVerifier *v, TypeNode *n);
static Status _verifyPtr           (EtnVerifier *v, TypeNode *n);
static Status _verifyArray         (EtnVerifier *v, TypeNode *n);
static Status _verifyInterface     (EtnVerifier *v, TypeNode *n);
static Status _verifyNotImplemented(EtnVerifier *v, TypeNode *n);

typedef struct VerifierMap_s {
    EtnKind kind;
    int scalarSize;
    Status (*verify) (EtnVerifier *v, TypeNode *n);
} VerifierMap;

// Later defined using ETN, but not yet available in types.
typedef uint8_t HashValue[64];

// NOTE: The order of the scalars here must
// match etnScalarSizeMap in types.c and types.go.
static const VerifierMap verifierMap[] = {
	{ EtnKindInt8,    1, _verifyScalar         },
	{ EtnKindUint8,   1, _verifyScalar         },
	{ EtnKindBool,    1, _verifyScalar         },
	{ EtnKindInt16,   2, _verifyScalar         },
	{ EtnKindUint16,  2, _verifyScalar         },
	{ EtnKindInt32,   4, _verifyScalar         },
	{ EtnKindUint32,  4, _verifyScalar         },
	{ EtnKindInt64,   8, _verifyScalar         },
	{ EtnKindUint64,  8, _verifyScalar         },
	{ EtnKindFloat32, 4, _verifyScalar         },
	{ EtnKindFloat64, 8, _verifyScalar         },
	{ EtnKindString,  0, _verifyString         },
	{ EtnKindAny,     0, _verifyAny            },
	{ EtnKindUnion,   0, _verifyUnion          },
	{ EtnKindType,    0, _verifyType           },
	{ EtnKindTuple,   0, _verifyTuple          },
	{ EtnKindArray,   0, _verifyArray          },
	{ EtnKindDict,    0, _verifyNotImplemented },
	{ EtnKindStruct,  0, _verifyStruct         },
	{ EtnKindIface,   0, _verifyInterface      },
	{ EtnKindFunc,    0, _verifyNotImplemented },
	{ EtnKindList,    0, _verifyStruct         },
	{ EtnKindPtr,     0, _verifyPtr            },
	{ EtnKindSptr,    0, _verifyNotImplemented },
};

static inline uint8_t *
_elemNhash (TypeNode *node, int n)
{
	// FIXME: See TypeNode.t FIXME. Should eventually rename TGHashValue and drop the trailing ->ptr below.
	return ((TGHashValue *) node->elems->ptr)[n]->ptr;
}

static Status
_decodeLength(EtnVerifier *v, EtnLength *l)
{
	ASSERT (v);
	ASSERT (l);

	EtnLength readSize = v->read(v, (uint8_t *) l, sizeof(EtnLength));
	if (readSize != sizeof(EtnLength)) {
		debugXPrint(typesDebug, "Could not read length\n");
		return StatusFail;
	}

	debugXPrint(typesDebug, "Decoded length: $[uint]\n", *l);

	return StatusOk;
}

static Status
_decodeCall(EtnVerifier *v, EtnCallId *c)
{
	ASSERT (v);
	ASSERT (c);

	EtnLength readSize = v->read(v, (uint8_t *) c, sizeof(EtnCallId));
	if (readSize != sizeof(EtnCallId)) {
		debugXPrint(typesDebug, "Could not read call\n");
		return StatusFail;
	}

	debugXPrint(typesDebug, "Decoded call: $[ulong]\n", *c);

	return StatusOk;
}

static Status
_decodeUnionId(EtnVerifier *v, EtnUnionId *id)
{
	ASSERT (v);
	ASSERT (id);

	EtnLength readSize = v->read(v, (uint8_t *) id, sizeof(uint64_t));
	if (readSize != sizeof(EtnUnionId)) {
		debugXPrint(typesDebug, "Could not read uinion ID\n");
		return StatusFail;
	}

	debugXPrint(typesDebug, "Decoded union ID: $[ulong]\n", *id);

	return StatusOk;
}

static Status
_decodePtrEncoding(EtnVerifier *v, EtnPtrEncoding *e)
{
	ASSERT (v);
	ASSERT (e);

	EtnLength readSize = v->read (v, (uint8_t *) e, sizeof (EtnPtrEncoding));
	if (readSize != sizeof(EtnPtrEncoding)) {
		debugXPrint(typesDebug, "Could not read pointer encoding\n");
		return StatusFail;
	}

	debugXPrint(typesDebug, "Decoded pointer encoding: $[uint]\n", *e);

	return StatusOk;
}

static Status
_decodeHash(EtnVerifier *v, Ref *s)
{
	ASSERT (v);
	REFCHECK (ucharXtype, s);
	ASSERT (HashSize == refSize (s));

	EtnLength len;
	Status status = _decodeLength(v, &len);

	if (StatusOk != status) {
		goto done;
	}

	if (len != HashSize) {
		status = StatusFail;
		goto done;
	}

	EtnLength sizeRead = v->read (v, s->ptr, HashSize);

	if (sizeRead != HashSize) {
		status = StatusFail;
		goto done;
	}

	debugXPrint(typesDebug, "Decoded hash: $[hash]\n", s->ptr);

done:
	return status;
}

static Status
_verifyScalar(EtnVerifier *v, TypeNode *n)
{
	v->objCount++;

	if (v->consume (v, verifierMap[n->base].scalarSize) != verifierMap[n->base].scalarSize) {
		debugXPrint(typesDebug, "Could not verify scalar\n");
		return StatusFail;
	}

	debugXPrint(typesDebug, "Verified scalar of size: $[uint]\n", verifierMap[n->base].scalarSize);

	return StatusOk;
}

static Status
_verifyString(EtnVerifier *v, TypeNode *n)
{
	EtnLength len;

	debugXPrint(typesDebug, "Verifying string\n");
	
	Status status = _decodeLength(v, &len);

	if (status != StatusOk) {
		goto done;
	}

	v->objCount++;

	if (v->consume (v, len) != len) {
		status = StatusFail;
		goto done;
	}

	debugXPrint(typesDebug, "Verified string\n");

done:
	return status;
}

static TypeNode *
_typeNodeForHash(uint8_t *hash, Slice typeGraph)
{
	int i;
	TypeNode *fnval = NULL;
	for (i = 0; i < typeGraph.len; i++) {
		if (NULL == typeGraph.array) {
			continue; // Ill-formed node.
		}
		TypeNode *node = &((TypeNode *) typeGraph.array)[i];
		// FIXME: See TypeNode.t FIXME. When HashValue becomes an array,
		//        drop the ->ptr below.
		if (! memcmp (node->hash->ptr, hash, HashSize)) {
			if (((TypeNode *)typeGraph.array)[i].base >= EtnKindInvalid) {
				continue; // Ill-formed node.
			}
			fnval = node;
		}
	}
	return fnval;
}

static Status
_verify(EtnVerifier *v, uint8_t *hash)
{
	Status status = StatusFail;

	TypeNode *node = _typeNodeForHash(hash, v->typeGraph);

	if (NULL != node) {
		debugXPrint(typesDebug, "Found hash in type graph; verifying $[string]\n", node->name);

		status = verifierMap[node->base].verify (v, node);
	}

	return status;
}

static Status
_verifyAny(EtnVerifier *v, TypeNode *n) 
{
	v->objCount++;

	Status status = StatusOk;
	Ref *hash = refBufferAllocate (HashSize);

	if (NULL == hash) {
		status = StatusNoMemory;
		goto done;
	}

	status = _decodeHash (v, hash);

	if (StatusOk != status) {
		goto done;
	}

	debugXPrint(typesDebug, "Verifying any (type hash: $[hash])\n", hash->ptr);

	status = _verify(v, hash->ptr);

	debugXPrint(typesDebug, "Verified any type\n");

done:
	if (NULL != hash) {
		refUnhook (&hash);
	}

	return status;
}

static Status
_verifyUnion(EtnVerifier *v, TypeNode *n)
{
	EtnUnionId id;

	v->objCount++;

	Status status = _decodeUnionId (v, &id);

	if (StatusOk != status) {
		goto done;
	}

	if (id >= refSize (n->elems)) {
		status = StatusNotImplemented;
		goto done;
	}

	debugXPrint(typesDebug, "Verifying union (type id: $[uint])\n", id);

	status = _verify(v, _elemNhash(n, id));

	debugXPrint(typesDebug, "Verified union type\n");

done:
	return status;
}

static Status
_verifyType(EtnVerifier *v, TypeNode *n)
{
	int i;
	for (i = 0; i < refSize (n->elems) / sizeof (String *); i++) {
		TypeNode *node = _typeNodeForHash(_elemNhash(n, i), v->typeGraph);
		if (StatusOk != verifierMap[node->base].verify (v, node)) {
			debugXPrint(typesDebug, "Could not verify type\n");
			return StatusFail;
		}
	}

	return StatusOk;
}

static Status
_verifyTuple(EtnVerifier *v, TypeNode *n)
{
	EtnLength len;

	debugXPrint(typesDebug, "Verifying tuple\n");

	Status status =  _decodeLength(v, &len);
	if (StatusOk != status) {
		goto done;
	}

	v->objCount++;

	TypeNode *node = _typeNodeForHash(_elemNhash(n, 0), v->typeGraph);
	int elemType = node->base;

	if (etnIsScalar(elemType)) {
		// Cheat.
		int size = len * etnScalarSizeMap[elemType];
		if (v->consume (v, size) != size) {
			debugXPrint(typesDebug, "Could not verify tuple (scalar)\n");
			status = StatusFail;
			goto done;
		}

	} else {
		int i;
		for (i = 0; i < len; i++) {
			if (StatusOk != verifierMap[node->base].verify (v, node)) {
				debugXPrint(typesDebug, "Could not verify tuple (non-scalar)\n");
				status = StatusFail;
				goto done;
			}
		}
	}

	debugXPrint(typesDebug, "Verified tuple of length: $[uint]\n", len);
done:
	return status;
}

static Status
_verifyStruct(EtnVerifier *v, TypeNode *n)
{
	v->objCount++;

	int i;
	debugXPrint(typesDebug, "Verifying struct\n");
	for (i = 0; i < refSize (n->elems) / sizeof (String *); i++) {
		TypeNode *node = _typeNodeForHash(_elemNhash(n, i), v->typeGraph);
		if (StatusOk != verifierMap[node->base].verify (v, node)) {
			debugXPrint(typesDebug, "Could not verify struct\n");
			return StatusFail;
		}
	}

	debugXPrint(typesDebug, "Verified struct with $[ulong] fields\n", refSize (n->elems));

	return StatusOk;
}

static Status
_verifyPtr(EtnVerifier *v, TypeNode *n)
{
        uint8_t typ;
        EtnLength idx;

	v->objCount++;

	Status status = _decodePtrEncoding(v, &typ);
        if (StatusOk != status) {
		goto done;
	}

        switch (typ) {
                case EtnPtrNil:
			debugXPrint(typesDebug, "Verified nil pointer\n");
                        break;

                case EtnPtrIndex:
			status = _decodeLength(v, &idx);
                        if (StatusOk != status) {
				goto done;
			}
			if (idx >= v->objCount) {
				status = StatusFail;
				goto done;
			}
			debugXPrint(typesDebug, "Verified pointer by index $[uint]\n", idx);
                        break;

                case EtnPtrInline:
			if (n->base >= EtnKindInvalid) {
				status = StatusFail;
				goto done;
			}
			debugXPrint(typesDebug, "Verifying pointer by value\n");
			TypeNode *node = _typeNodeForHash(_elemNhash(n, 0), v->typeGraph);
			status = verifierMap[node->base].verify (v, node);
			debugXPrint(typesDebug, "Verified pointer by value\n");
                        break;
                default:
			status = StatusNotImplemented;
                        break;
        }

done:
	return status;
}

static Status
_verifyArray(EtnVerifier *v, TypeNode *n)
{
	Status status = StatusOk;
	debugXPrint(typesDebug, "Verifying array\n");

	v->objCount++;

	TypeNode *node = _typeNodeForHash(_elemNhash(n, 0), v->typeGraph);
	int elemType = node->base;

	if (etnIsScalar(elemType)) {
		// Cheat.
		int size = n->size * etnScalarSizeMap[elemType];
		if (v->consume (v, size) != size) {
			debugXPrint(typesDebug, "Could not verify array (scalar)\n");
			status = StatusFail;
			goto done;
		}

	} else {
		int i;
		for (i = 0; i < n->size; i++) {
			status = verifierMap[node->base].verify (v, node);
			if (StatusOk != status) {
				debugXPrint(typesDebug, "Could not verify array (non-scalar)\n");
				goto done;
			}
		}
	}

	debugXPrint(typesDebug, "Verified array\n");
done:
	return status;
}

static Status
_verifyInterface(EtnVerifier *v, TypeNode *n)
{

	uint64_t call;

	debugXPrint(typesDebug, "Verifying interface\n");

	Status status = _decodeCall(v, &call);

	if (StatusOk != status) {
		goto done;
	}

	// Ensure call is < # methods in interface.
	if (call >= refSize (n->elems) / sizeof (String *)) {
		status = StatusNotImplemented;
		goto done;
	}

	uint8_t *methodHash = _elemNhash(n, call);
	TypeNode *methodNode = _typeNodeForHash(methodHash, v->typeGraph);
	if (EtnKindFunc != methodNode->base) {
		status = StatusFail;
		goto done;
	}

	uint8_t *nodeHash = _elemNhash(methodNode, 0);
	n = _typeNodeForHash(nodeHash, v->typeGraph);

	status = verifierMap[n->base].verify (v, n);

	debugXPrint(typesDebug, "Verified interface\n");

done:
	return status;
}

static Status
_verifyNotImplemented(EtnVerifier *v, TypeNode *n)
{
	debugXPrint(typesDebug, "Verification of 0x$[hexint] not implemented\n", n->base);

	return StatusNotImplemented;
}

Status
etnVerify(EtnVerifier *v, uint8_t *hash) 
{
	v->objCount = 0;
	Status status = _verify(v, hash);
	if (StatusOk != status) {
		debugXPrint(typesDebug, "Could not verify\n");
		return status;
	}
	// Should be nothing left.
	EtnLength left = v->consume(v, 1);
	debugXPrint(typesDebug, "Remaining data to verify: $[cstring]\n", left ? "yes" : "no");
	return left == 0 ? StatusOk : StatusFail;
}

// A consume function that consumes from memory.
static int
_bufferVerifierConsume(EtnVerifier * _v, EtnLength length)
{
	EtnBufferVerifier *v = (EtnBufferVerifier *) _v;
	length = v->dataCurrent + length < v->dataEnd ? length : v->dataEnd - v->dataCurrent;
	v->dataCurrent += length;
	return length;
}

// A read function that reads from memory.
static int
_bufferVerifierRead(EtnVerifier * _v, uint8_t *data, EtnLength length)
{
	ASSERT(data);

	EtnBufferVerifier *v = (EtnBufferVerifier *) _v;
	length = v->dataCurrent + length < v->dataEnd ? length : v->dataEnd - v->dataCurrent;
	memcpy (data, v->dataCurrent, length);
	v->dataCurrent += length;
	return length;
}

// Generic reset; should only be called by subclasses.
void
etnVerifierReset(EtnVerifier *v, int (*consume) (EtnVerifier *v, EtnLength length), int (*read) (EtnVerifier *v, uint8_t *data, EtnLength length))
{
	ASSERT(v);
	v->objCount = 0;
	v->consume  = consume;
	v->read = read;
}

// Reset buffer decoder and use a newly provided buffer.
void
etnBufferVerifierReset(EtnBufferVerifier *v, uint8_t *data, EtnLength length)
{
	ASSERT(v);
	etnVerifierReset ((EtnVerifier *) v, _bufferVerifierConsume, _bufferVerifierRead);
	v->dataCurrent = data;
	v->dataEnd     = data + length;
}

// Create a new buffer verifier.
EtnBufferVerifier *
etnBufferVerifierNew(Slice graph, uint8_t *data, EtnLength length)
{
	EtnBufferVerifier *v = malloc (sizeof (EtnBufferVerifier));
	ASSERT(v);
	etnBufferVerifierReset(v, data, length);

	v->verifier.typeGraph = graph;

	return v;
}
