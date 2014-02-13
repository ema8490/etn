#include <ethos/dual/ref.h>
#include <ethos/dual/string.h>
#include <ethos/generic/assert.h>
#include <ethos/generic/debug.h>
#include <ethos/generic/tree.h>
#include <ethos/generic/etn.h>
#include <endian.h>
#include <stdint.h>

#if __BYTE_ORDER == __BIG_ENDIAN
#error Big endian not presently supported; need to byteswap scalars (and see etnIsScalar), etc.
#endif

/* When encoding, we must remember pointers to some encoded types so that we
 * can handle type loops (e.g., A->B->C->A). But for performance reasons (it
 * takes time to enter the pointer into our data structure and look it up)
 * we don't want to remember all pointers. So, we remember the pointer to
 * the type that was passed to encode() and we remember all EtnKindPtrs and EtnKindAnys.
 * EtnKindPtrs may point to another type (e.g., B in the chain above).
 */
enum {
	RememberPointer = true,
	NoRememberPointer = false
};

static Status _encodeScalar        (EtnEncoder *e, EtnValue v, bool rememberPointer, EtnLength *length);
static Status _encodePtr           (EtnEncoder *e, EtnValue v, bool rememberPointer, EtnLength *length);
static Status _encodeTuple         (EtnEncoder *e, EtnValue v, bool rememberPointer, EtnLength *length);
static Status _encodeArray         (EtnEncoder *e, EtnValue v, bool rememberPointer, EtnLength *length);
static Status _encodeAny           (EtnEncoder *e, EtnValue v, bool rememberPointer, EtnLength *length);
static Status _encodeUnion         (EtnEncoder *e, EtnValue v, bool rememberPointer, EtnLength *length);
static Status _encodeStruct        (EtnEncoder *e, EtnValue v, bool rememberPointer, EtnLength *length);
static Status _encodeNotImplemented(EtnEncoder *e, EtnValue v, bool rememberPointer, EtnLength *length);

typedef struct EncoderMap {
    EtnKind kind;
    Status (*encode) (EtnEncoder *e, EtnValue v, bool rememberPointer, EtnLength *length);
} EncoderMap;

static const EncoderMap encoderMap[] = {
        { EtnKindInt8,    _encodeScalar         },
        { EtnKindUint8,   _encodeScalar         },
        { EtnKindBool,    _encodeScalar         },
        { EtnKindInt16,   _encodeScalar         },
        { EtnKindUint16,  _encodeScalar         },
        { EtnKindInt32,   _encodeScalar         },
        { EtnKindUint32,  _encodeScalar         },
        { EtnKindInt64,   _encodeScalar         },
        { EtnKindUint64,  _encodeScalar         },
        { EtnKindFloat32, _encodeScalar         },
        { EtnKindFloat64, _encodeScalar         },
        { EtnKindString,  _encodeTuple          },
        { EtnKindAny,     _encodeAny            },
        { EtnKindUnion,   _encodeUnion          },
        { EtnKindType,    _encodeNotImplemented },
        { EtnKindTuple,   _encodeTuple          },
        { EtnKindArray,   _encodeArray          },
        { EtnKindDict,    _encodeNotImplemented },
        { EtnKindStruct,  _encodeStruct         },
        { EtnKindIface,   _encodeNotImplemented },
        { EtnKindFunc,    _encodeNotImplemented },
        { EtnKindList,    _encodeNotImplemented },
        { EtnKindPtr,     _encodePtr            },
        { EtnKindSptr,    _encodeNotImplemented },
};

typedef struct AddrToIndex_s {
    void *ptr;
    EtnLength index;
    struct rbtree_node node;
} AddrToIndex;

static EtnPtrEncoding pnil = EtnPtrNil, pidx = EtnPtrIndex, pval = EtnPtrInline;

static int 
_compareFn (const struct rbtree_node *node1, const struct rbtree_node *node2)
{
    const AddrToIndex *n1 = rbtree_container_of(node1, AddrToIndex, node);
    const AddrToIndex *n2 = rbtree_container_of(node2, AddrToIndex, node);

    return n1->ptr - n2->ptr;
}

static void
_freeTree (struct rbtree *tree)
{
    struct rbtree_node *node = rbtree_first(tree);
    while (node)
	{
	    AddrToIndex *aiNode = rbtree_container_of (node, AddrToIndex, node);
	    node = rbtree_next (node);
	    free (aiNode);
	}
}

static void
_reset (EtnEncoder *e)
{
    ASSERT (e);
    _freeTree (&e->addrToIndex);
    rbtree_init (&e->addrToIndex, _compareFn, 0);
}

static Status
_encodeLength(EtnEncoder *e, EtnLength l)
{
    ASSERT (e);

    EtnLength writeSize = e->write(e, (uint8_t *) &l, sizeof(l));
    if (writeSize != sizeof (l))
	{
	    return StatusFail;
	}

    debugXPrint(typesDebug, "Encoded length: $[uint]\n", l);

    return StatusOk;
}

// Remember a pointer, if necessary.
static void
_maybeRememberPointer(EtnEncoder *e, EtnValue v, bool rememberPointer)
{
    if (rememberPointer == true && v.data != e->topLevelPointer)
	{
	    AddrToIndex query;
	    query.ptr = v.data;
	    struct rbtree_node *n = rbtree_lookup(&query.node, &e->addrToIndex);
	    if (n == NULL)
		{
		    AddrToIndex *node = malloc (sizeof (AddrToIndex));
		    ASSERT (node);

		    node->ptr = v.data;
		    node->index = e->index;

		    rbtree_insert(&node->node, &e->addrToIndex);

		    debugXPrint(typesDebug, "Remember pointer/index: $[pointer]/$[uint]\n", node->ptr, node->index);
		}
	}
}

// Encode a scalar.
static Status
_encodeScalar(EtnEncoder *e, EtnValue v, bool rememberPointer, EtnLength *length)
{
    ASSERT (e);
    ASSERT (v.type);
    ASSERT (v.data);
    ASSERT (length);

    _maybeRememberPointer(e, v, rememberPointer);
    e->index++;

    *length = e->write(e, v.data, v.type->size);

    if (*length != v.type->size)
	{
	    return StatusFail;
	}

    debugXPrint(typesDebug, "Encoded scalar of size: $[uint]\n", v.type->size);

    return StatusOk;
}

// Encode a pointer.
static Status
_encodePtr(EtnEncoder *e, EtnValue v, bool rememberPointer, EtnLength *length)
{
	ASSERT (e);
	ASSERT (v.type);
	ASSERT (v.data);
	ASSERT (length);

	Status status;
	EtnLength index = 0;

	_maybeRememberPointer(e, v, rememberPointer);
	e->index++;

	if (*(void **)v.data == 0)
	    {
		// NULL.
		status = encoderMap[EtnKindUint8].encode (e, EtnToValue(&PtrEncodingType, &pnil), NoRememberPointer, length);
		debugXPrint(typesDebug, "Encoded nil pointer\n");
		return status;
	    }

	// See elsewhere for discussion of e->topLevelPointer.
	// Extra logic is due to a performance optimization.
	if (*(void **)v.data != e->topLevelPointer)
	    {
		AddrToIndex query = { .ptr = *(void **)v.data };
		struct rbtree_node *tr = rbtree_lookup(&query.node, &e->addrToIndex);
		if (tr)
		    {
			index = rbtree_container_of(tr, AddrToIndex, node)->index;
		    }
	    }

	if ((*(void **)v.data == e->topLevelPointer) || index)
	     // In former case, index is zero.
	     // Otherwise index == 0 implies not yet seen.
	    {
		// Previously seen; encode index.
		status = encoderMap[EtnKindUint8].encode (e, EtnToValue(&PtrEncodingType, &pidx), NoRememberPointer, length);
		if (StatusOk != status)
		    {
			return status;
		    }
		
		status = _encodeLength(e, index);
		if (StatusOk != status)
		    {
			return status;
		    }
		*length += sizeof (EtnLength);
		debugXPrint(typesDebug, "Encoded pointer $[pointer] by index $[uint]\n", *(void **) v.data, index);
	    }
	else 
	    {
		// New; encode inline.
		debugXPrint(typesDebug, "Encoding pointer $[pointer] by value\n", *(void **) v.data);
		status = encoderMap[EtnKindUint8].encode (e, EtnToValue(&PtrEncodingType, &pval), RememberPointer, length);
		if (StatusOk != status)
		    {
			return status;
		    }

		EtnIndirectType *c = (EtnIndirectType *)v.type;
		if (c->elem->kind >= EtnKindInvalid)
		    {
			return StatusFail;
		    }

		EtnLength _length;
		status = encoderMap[c->elem->kind].encode (e, EtnToValue(c->elem, *(void **)v.data), RememberPointer, &_length);
		if (StatusOk != status)
		    {
			return status;
		    }
		*length += _length;
		debugXPrint(typesDebug, "Encoded pointer $[pointer] by value\n", *(void **) v.data);
	    }

	return status;
}

// Encode a tuple.
static Status
_encodeTuple(EtnEncoder *e, EtnValue v, bool rememberPointer, EtnLength *length)
{
	ASSERT (e);
	ASSERT (v.type);
	ASSERT (v.data);
	ASSERT (length);

	debugXPrint(typesDebug, "Encoding tuple\n");

	EtnLength _length = *length = 0;

	_maybeRememberPointer(e, v, rememberPointer);
	e->index++;

	Ref **s = (Ref **)v.data;
	uint8_t *data = (*s)->ptr;
	EtnIndirectType *c = (EtnIndirectType *)v.type;
	Status status = _encodeLength(e, refSize (*s) / c->elem->size);
	if (StatusOk != status)
	    {
		return status;
	    }
	*length += sizeof (EtnLength);
	if (etnIsScalar(c->elem->kind)) {
		// Cheat.
		EtnLength writeSize = e->write(e, data, refSize(*s));
		if (writeSize != refSize(*s))
		    {
			return StatusFail;
		    }
		*length += writeSize;
	} else {
		int i;
		for (i = 0; i < refSize (*s) / c->elem->size; i++, data += c->elem->size) {
			if (c->elem->kind >= EtnKindInvalid)
			    {
				return StatusFail;
			    }
			status = encoderMap[c->elem->kind].encode (e, EtnToValue(c->elem, data), NoRememberPointer, &_length);
			if (StatusOk != status)
			    {
				return status;
			    }
			*length += _length;
		}
	}

	debugXPrint(typesDebug, "Encoded tuple of size: $[uint] * $[uint]\n", refSize (*s), c->elem->size);

	return StatusOk;
}

// Encode an array.
static Status
_encodeArray(EtnEncoder *e, EtnValue v, bool rememberPointer, EtnLength *length)
{
	ASSERT (e);
	ASSERT (v.type);
	ASSERT (v.data);
	ASSERT (length);

	debugXPrint(typesDebug, "Encoding array\n");

	EtnLength _length = *length = 0;

	_maybeRememberPointer(e, v, rememberPointer);
	e->index++;

	int i;
	EtnIndirectType *c = (EtnIndirectType *)v.type;
	if (etnIsScalar(c->elem->kind)) {
		// Cheat.
		EtnLength writeSize = e->write(e, v.data, c->type.size * c->elem->size);
		if (writeSize != c->type.size * c->elem->size)
		    {
			return StatusFail;
		    }
		*length += writeSize;
	} else {
		for (i = 0; i < c->type.size; i++, v.data += c->elem->size) {
			if (c->elem->kind >= EtnKindInvalid)
			    {
				return StatusFail;
			    }
			Status status = encoderMap[c->elem->kind].encode (e, EtnToValue(c->elem, v.data), NoRememberPointer, &_length);
			if (StatusOk != status)
			    {
				return status;
			    }
			*length += _length;
		}
	}

	debugXPrint(typesDebug, "Encoded array of size: $[uint] * $[uint]\n", c->type.size, c->elem->size);

	return StatusOk;
}

// Encode an any type.
static Status
_encodeAny(EtnEncoder *e, EtnValue v, bool rememberPointer, EtnLength *length)
{
	ASSERT (e);
	ASSERT (v.type);
	ASSERT (v.data);
	ASSERT (length);

	String *hash = NULL;
	Status status = StatusOk;
	EtnLength _length = *length = 0;

	_maybeRememberPointer(e, v, rememberPointer);
	e->index++;

	uint8_t h[HashSize];
	v = *(EtnValue *)v.data;
	status = etnHashForType(v.type, h);
	if (StatusOk != status) {
		goto done;
	}

	hash = refBufferAllocateInitialize(HashSize,h);
	if (NULL == hash) {
		status = StatusNoMemory;
		goto done;
	}


	debugXPrint(typesDebug, "Encoding any (type hash: $[hash])\n", hash->ptr);
	status = encoderMap[EtnKindTuple].encode (e, EtnToValue(&HashType, &hash), RememberPointer, &_length);
	if (StatusOk != status) {
		goto done;
	}
	*length += _length;
	if (v.type->kind >= EtnKindInvalid) {
		status = StatusFail;
		goto done;
	}
	status = encoderMap[v.type->kind].encode (e, v, RememberPointer, &_length);
	if (StatusOk != status) {
		goto done;
	}
	*length += _length;
	debugXPrint(typesDebug, "Encoded any type\n");

done:
	if (NULL != hash) {
		stringUnhook (&hash);
	}

	return StatusOk;
}

// Encode a union.
static Status
_encodeUnion(EtnEncoder *e, EtnValue v, bool rememberPointer, EtnLength *length)
{
	ASSERT (e);
	ASSERT (v.type);
	ASSERT (v.data);
	ASSERT (length);

	_maybeRememberPointer(e, v, rememberPointer);
	e->index++;

	debugXPrint(typesDebug, "Encoding union type\n");

	EtnLength _length = *length = 0;
	Status status = StatusFail;
	EtnUnionId id = 0;
	for (id = 0; id < ((EtnUnionType *)v.type)->len; id++) {
		EtnValue actualV = *(EtnValue *)v.data;
		if (actualV.type == ((EtnUnionType *)v.type)->fields[id].elem) {
			status = encoderMap[EtnKindUnionId].encode (e, EtnToValue(&UnionIdType, &id), rememberPointer, &_length);
			if (StatusOk != status)
			    {
				return status;
			    }
			*length += _length;
			if (v.type->kind >= EtnKindInvalid)
			    {
				return StatusFail;
			    }
			status = encoderMap[actualV.type->kind].encode (e, actualV, rememberPointer, &_length);
			if (StatusOk != status)
			    {
				return status;
			    }
			*length += _length;
			debugXPrint(typesDebug, "Encoded union type\n");
			return status;
		}
	}
	return StatusFail;
}

// Encode a struct.
static Status
_encodeStruct(EtnEncoder *e, EtnValue v, bool rememberPointer, EtnLength *length)
{
	ASSERT (e);
	ASSERT (v.type);
	ASSERT (v.data);
	ASSERT (length);

	EtnLength _length = *length = 0;

	_maybeRememberPointer(e, v, rememberPointer);
	e->index++;

	int i;
	Status status = StatusOk;
	EtnStructType *c = (EtnStructType *)v.type;
	debugXPrint(typesDebug, "Encoding struct\n");
	for (i = 0; i < c->len; i++) {
		if (c->fields[i].elem->kind >= EtnKindInvalid) {
			return StatusFail;
		}
		status = encoderMap[c->fields[i].elem->kind].encode (e, EtnToValue(c->fields[i].elem, v.data + c->fields[i].offset), NoRememberPointer, &_length);
		if (StatusOk != status)
		    {
			return status;
		    }
		*length += _length;
	}
	debugXPrint(typesDebug, "Encoded struct with $[uint] fields\n", c->len);

	return status;
}

static EtnLength
_encodeNotImplemented(EtnEncoder *e, EtnValue v, bool rememberPointer, EtnLength *length)
{
	ASSERT (e);
	ASSERT (v.type);
	ASSERT (v.data);
	ASSERT (length);

	_maybeRememberPointer(e, v, rememberPointer);
	e->index++;

	return StatusNotImplemented;
}

// Encode an arbitrary type.
Status
etnEncodeWithoutFlush(EtnEncoder *e, EtnValue v, EtnLength *length) 
{
    if (v.type->kind >= EtnKindInvalid)
	{
	    return StatusFail;
	}

    e->index = 0;
    // Store top-level pointer out of e->addrToIndex tree.
    // This avoids a malloc for a simple type that contains
    // no pointers or any types (we must malloc to insert
    // into tree).
    e->topLevelPointer = v.data;

    Status status = encoderMap[v.type->kind].encode (e, v, NoRememberPointer, length);
    _reset (e);

    return status;
}

// Encode an arbitrary type.
Status
etnEncode(EtnEncoder *e, EtnValue v, EtnLength *length) 
{
    Status status = etnEncodeWithoutFlush(e, v, length);

    e->flush(e);

    return status;
}

// Generic reset; should only be called by subclasses.
void
etnEncoderReset(EtnEncoder *e, int (*write) (EtnEncoder *e, uint8_t *data, EtnLength length), void (*flush) (EtnEncoder *e))
{
    ASSERT (e);
    e->write = write;
    e->flush = flush;
    rbtree_init (&e->addrToIndex, _compareFn, 0);
}

// NOTE: This is an unadvertised but exported function
//       used by etnEthosEncoderWrite. Maybe etnEthos
//       should move here.
int
etnBufferEncoderWrite(EtnEncoder *_e, uint8_t *data, EtnLength length)
{
    ASSERT(_e);
    ASSERT(data || length == 0);

    EtnBufferEncoder *e = (EtnBufferEncoder *) _e;

    ASSERT(e->dataCurrent + length <= e->dataEnd);

    // FIXME: Copied into decoder.c.
    // Try to take advantage of word-sized copies;
    // benchmarks indicates this helps.
    EtnLength remaining = length;
    if (remaining % sizeof(long) == 0)
	{
	    for (; remaining >= sizeof(long); remaining -= sizeof(long))
		{
		    *(long *) e->dataCurrent = *(long *) data;
		    e->dataCurrent += sizeof(long);
		    data += sizeof(long);
		}
	}

    if (remaining)
	{
	    memcpy(e->dataCurrent, data, remaining);
	    e->dataCurrent += remaining;
	}

    return length;
}

static void
etnBufferEncoderFlush(EtnEncoder *e)
{
}

// Reset buffer encoder and use a newly provided buffer.
void
etnBufferEncoderReset(EtnBufferEncoder *e, uint8_t *data, EtnLength length)
{
    ASSERT (e);
    etnEncoderReset ((EtnEncoder *) e, etnBufferEncoderWrite, etnBufferEncoderFlush);
    e->dataCurrent = data;
    e->dataEnd     = data + length;
}

// Create a new buffer encoder.
EtnBufferEncoder *
etnBufferEncoderNew(uint8_t *data, EtnLength length)
{
    EtnBufferEncoder *e = malloc (sizeof (EtnBufferEncoder));
    ASSERT(e);
    etnBufferEncoderReset(e, data, length);
    return e;
}

static int
etnNullEncoderWrite(EtnEncoder *_e, uint8_t *data, EtnLength length)
{
    ASSERT(data || length == 0);

    return length;
}

static void
etnNullEncoderFlush(EtnEncoder *e)
{
}

// Reset null encoder.
void
etnNullEncoderReset(EtnNullEncoder *e)
{
    ASSERT (e);
    etnEncoderReset ((EtnEncoder *) e, etnNullEncoderWrite, etnNullEncoderFlush);
}

// Create a new null encoder.
EtnNullEncoder *
etnNullEncoderNew(void)
{
    EtnNullEncoder *e = malloc (sizeof (EtnNullEncoder));
    ASSERT(e);
    etnNullEncoderReset(e);
    return e;
}
