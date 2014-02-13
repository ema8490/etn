#include <ethos/dual/string.h>
#include <ethos/generic/assert.h>
#include <ethos/generic/debug.h>
#include <ethos/generic/etn.h>
#include <ethos/dual/ref.h>
#include <endian.h>

#if __BYTE_ORDER == __BIG_ENDIAN
#error Big endian not presently supported; need to byteswap scalars (and see etnIsScalar), etc.
#endif

enum {
	MinIndexToDataLength = 128
};

static Status _decodeScalar        (EtnDecoder *d, EtnValue v);
static Status _decodePtr           (EtnDecoder *d, EtnValue v);
static Status _decodeString        (EtnDecoder *d, EtnValue v);
static Status _decodeTuple         (EtnDecoder *d, EtnValue v);
static Status _decodeArray         (EtnDecoder *d, EtnValue v);
static Status _decodeAny           (EtnDecoder *d, EtnValue v);
static Status _decodeUnion         (EtnDecoder *d, EtnValue v);
static Status _decodeStruct        (EtnDecoder *d, EtnValue v);
static Status _decodeNotImplemented(EtnDecoder *d, EtnValue v);

typedef struct DecoderMap_s {
    EtnKind kind;
    Status (*decode) (EtnDecoder *d, EtnValue v);
} DecoderMap;

static const DecoderMap decoderMap[] = {
	{ EtnKindInt8,    _decodeScalar         },
	{ EtnKindUint8,   _decodeScalar         },
	{ EtnKindBool,    _decodeScalar         },
	{ EtnKindInt16,   _decodeScalar         },
	{ EtnKindUint16,  _decodeScalar         },
	{ EtnKindInt32,   _decodeScalar         },
	{ EtnKindUint32,  _decodeScalar         },
	{ EtnKindInt64,   _decodeScalar         },
	{ EtnKindUint64,  _decodeScalar         },
	{ EtnKindFloat32, _decodeScalar         },
	{ EtnKindFloat64, _decodeScalar         },
	{ EtnKindString,  _decodeString         },
	{ EtnKindAny,     _decodeAny            },
	{ EtnKindUnion,   _decodeUnion          },
	{ EtnKindType,    _decodeNotImplemented },
	{ EtnKindTuple,   _decodeTuple          },
	{ EtnKindArray,   _decodeArray          },
	{ EtnKindDict,    _decodeNotImplemented },
	{ EtnKindStruct,  _decodeStruct         },
	{ EtnKindIface,   _decodeNotImplemented },
	{ EtnKindFunc,    _decodeNotImplemented },
	{ EtnKindList,    _decodeNotImplemented },
	{ EtnKindPtr,     _decodePtr            },
	{ EtnKindSptr,    _decodeNotImplemented },
};

// Store an address so that we may latter look it up by index
// when processing a pointer; grow the array as needed.
static void
_updateIndexToData(EtnDecoder *d, EtnValue v)
{
    if (d->nextIndex >= d->indexToDataLength) 
	{
	    void **n;
	    unsigned long newLen = d->indexToDataLength * 2 > d->nextIndex ? d->indexToDataLength * 2 : d->nextIndex;
	    n = malloc(newLen * sizeof(void *));
	    ASSERT (n);
	    memcpy(n, d->indexToData, d->indexToDataLength * sizeof(void *));
	    if (d->indexToDataLength > MinIndexToDataLength)
		{
		    // Initialy uses static buffer; only malloc'ed if it needed to grow.
		    free(d->indexToData);
		}
	    d->indexToDataLength = newLen;
	    d->indexToData = n;
	}
    d->indexToData[d->nextIndex] = v.data;
    d->nextIndex++;
}

static Status
_decodeLength(EtnDecoder *d, EtnLength *l)
{
    EtnLength sizeRead = d->read(d, (uint8_t *) l, sizeof(EtnLength));
    if (sizeRead != sizeof(EtnLength))
	{
	    return StatusFail;
	}

    debugXPrint(typesDebug, "Decoded length: $[uint]\n", *l);

    return StatusOk;
}

// Decode a scalar, but do not store its index/address. This is needed to
// decode a pointer type (i.e., EtnPtrNil, EtnPtrIndex, or EtnPtrInline); these are not indexed.
static Status
_decodeScalarWithoutUpdatingIndex(EtnDecoder *d, EtnValue v)
{
	ASSERT (d);
	ASSERT (v.type);
	ASSERT (v.data);

	EtnLength sizeRead = d->read (d, v.data, v.type->size);
	if (sizeRead != v.type->size) {
		return StatusFail;
	}

	debugXPrint(typesDebug, "Decoded scalar of size: $[uint]\n", v.type->size);

	return StatusOk;
}

// Decode a scalar value to EtnValue v.
static Status
_decodeScalar(EtnDecoder *d, EtnValue v)
{
	ASSERT (d);
	ASSERT (v.type);
	ASSERT (v.data);

	_updateIndexToData(d, v);
	return _decodeScalarWithoutUpdatingIndex(d, v);
}

// Decode an array to EtnValue v.
static Status
_decodeArray(EtnDecoder *d, EtnValue v)
{
	ASSERT (d);
	ASSERT (v.type);
	ASSERT (v.data);

	debugXPrint(typesDebug, "Decoding array\n");

	int i;
	Status status = StatusOk;
	EtnIndirectType *c = (EtnIndirectType *)v.type;

	_updateIndexToData(d, v);

	if (etnIsScalar(c->elem->kind)) {
                // Cheat.
		EtnLength sizeRead = d->read (d, v.data, c->type.size * c->elem->size);
		if (sizeRead != c->type.size * c->elem->size) {
			return StatusFail;
		}
		d->nextIndex += c->type.size;
	} else {
                for (i = 0; i < c->type.size; i++, v.data += c->elem->size) {
			if (c->elem->kind >= EtnKindInvalid) {
				return StatusFail;
			}
			status = decoderMap[c->elem->kind].decode (d, EtnToValue(c->elem, v.data));
			if (StatusOk != status) {
				return status;
			}
		}
	}

	debugXPrint(typesDebug, "Decoded array of size: $[uint] * $[uint]\n", c->type.size, c->elem->size);

	return status;
}

// Decode a string to EtnValue v.
static Status
_decodeString(EtnDecoder *d, EtnValue v)
{
	ASSERT (d);
	ASSERT (v.type);
	ASSERT (v.data);

	debugXPrint(typesDebug, "Decoding string\n");

	Status status;
	String **s = (String **)v.data;
	*s = NULL;
	EtnIndirectType *c = (EtnIndirectType *)v.type;
	ASSERT (c->elem->size == 1); // Should be chars.

	_updateIndexToData(d, v);

	EtnLength len;
	status = _decodeLength(d, &len);
	if (StatusOk != status) {
		goto done;
	}

	*s = stringAllocate(len);
	if (NULL == *s) {
		status = StatusNoMemory;
		goto done;
	}

	if (len > 0) {

		// Cheat.
		// FIXME: This breaks abstraction of (*s)->ptr!
		EtnLength sizeRead = d->read (d, (*s)->ptr, len);
		if (sizeRead != len) {
			status = StatusFail;
			goto done;
		}
		((char *) (*s)->ptr)[len] = 0x00;
	}
	
	// This is why _decodeString is separate from _decodeTuple.
	// Go ETN does not save a pointer to each character in a string.
	d->nextIndex += len;

	debugXPrint(typesDebug, "Decoded string: $[string]\n", *s);

done:
	if (StatusOk != status && NULL != *s) {
		stringUnhook (s);
	}

	return status;
}

// Decode a tuple to EtnValue v.
static Status
_decodeTuple(EtnDecoder *d, EtnValue v)
{
	ASSERT (d);
	ASSERT (v.type);
	ASSERT (v.data);

	debugXPrint(typesDebug, "Decoding tuple\n");

	Status status;
	Ref **s = (Ref **)v.data;
	*s = NULL;
	EtnIndirectType *c = (EtnIndirectType *)v.type;

	_updateIndexToData(d, v);

	EtnLength len;
	status = _decodeLength(d, &len);
	if (StatusOk != status) {
		goto done;
	}

	*s = refBufferAllocate(len * c->elem->size);
	if (NULL == *s) {
		status = StatusNoMemory;
		goto done;
	}

	if (len > 0) {
		if (etnIsScalar(c->elem->kind)) {
			// Cheat.
			// FIXME: This breaks abstraction of (*s)->ptr!
			EtnLength sizeRead = d->read (d, (*s)->ptr, len * c->elem->size);
			if (sizeRead != len * c->elem->size) {
				status = StatusFail;
				goto done;
			}
			d->nextIndex += len;
		} else {
			int i;
			uint8_t *p;
			// FIXME: This breaks abstraction of (*s)->ptr!
			for (i = 0, p = (*s)->ptr; i < len; i++, p += c->elem->size) {
				if (c->elem->kind >= EtnKindInvalid) {
					status = StatusFail;
					goto done;
				}
				status = decoderMap[c->elem->kind].decode (d, EtnToValue(c->elem, p));
				if (StatusOk != status) {
					goto done;
				}
			}
		}
	}

	debugXPrint(typesDebug, "Decoded tuple of size: $[uint] * $[uint]\n", len, c->elem->size);

done:
	if (StatusOk != status && NULL != *s) {
		refUnhook (s);
	}

	return status;
}

// Decode a struct to EtnValue v.
static Status
_decodeStruct(EtnDecoder *d, EtnValue v)
{
	ASSERT (d);
	ASSERT (v.type);
	ASSERT (v.data);

	_updateIndexToData(d, v);

	int i;
	Status status = StatusOk;
	EtnStructType *c = (EtnStructType *)v.type;
	debugXPrint(typesDebug, "Decoding struct\n");
	for (i = 0; i < c->len; i++) {
		if (c->fields[i].elem->kind >= EtnKindInvalid) {
			return StatusFail;
		}
		status = decoderMap[c->fields[i].elem->kind].decode (d, EtnToValue(c->fields[i].elem, v.data + c->fields[i].offset));
		if (StatusOk != status) {
			return status;
		}
	}

	debugXPrint(typesDebug, "Decoded struct with $[uint] fields\n", c->len);

	return status;
}

// Decoder a pointer to EtnValue v.
static Status
_decodePtr(EtnDecoder *d, EtnValue v)
{
	ASSERT (d);
	ASSERT (v.type);
	ASSERT (v.data);

	EtnPtrEncoding encoding;
	EtnLength idx;
	EtnIndirectType *c = (EtnIndirectType *)v.type;

	_updateIndexToData(d, v);

	Status status = _decodeScalarWithoutUpdatingIndex(d, EtnToValue(&PtrEncodingType, &encoding));
	if (StatusOk != status) {
		return status;
	}

	switch (encoding) {
		case EtnPtrNil:
			// NULL.
			*(uint8_t **)v.data = 0;
			debugXPrint(typesDebug, "Decoded nil pointer\n");
			break;
		case EtnPtrIndex:
			// Previously seen; lookup address for decoded index.
			status = _decodeLength(d, &idx);
			if (StatusOk != status) {
				return status;
			}
			if (idx >= d->nextIndex) {
				return StatusFail;
			}
			*(uint8_t **)v.data = d->indexToData[idx];
			debugXPrint(typesDebug, "Decoded pointer $[pointer] by index $[uint]\n", *(void **) v.data, idx);
			break;
		case EtnPtrInline:
			// New; decode value which is provided inline.
			*(uint8_t **)v.data = malloc(c->elem->size);
			if (NULL == *(uint8_t **)v.data) {
				return StatusNoMemory;
			}
			if (c->elem->kind >= EtnKindInvalid) {
				free (*(uint8_t **)v.data);
				return StatusFail;
			}
			debugXPrint(typesDebug, "Decoding pointer $[pointer] by value\n", *(void **) v.data);
			status = decoderMap[c->elem->kind].decode (d, EtnToValue(c->elem, *(uint8_t **)v.data));
			if (StatusOk != status) {
				free (*(uint8_t **)v.data);
				return status;
			}
			debugXPrint(typesDebug, "Decoded pointer $[pointer] by value\n", *(void **) v.data);
			break;
		default:
			debugXPrint(typesDebug, "Unknown pointer encoding: $[uint]\n", encoding);
			status = StatusNotImplemented;
			break;
	}

	return status;
}

// Decode an any type into EtnValue v.
static Status
_decodeAny(EtnDecoder *d, EtnValue v)
{
	ASSERT (d);
	ASSERT (v.type);
	ASSERT (v.data);

	Status status;
	HashString *hash;
	EtnValue *vv = (EtnValue *)v.data;
	vv->data = NULL;

	_updateIndexToData(d, v);

	status = decoderMap[EtnKindTuple].decode (d, EtnToValue(&HashType, &hash));
	if (StatusOk != status) {
		goto done;
	}
	debugXPrint(typesDebug, "Decoding any (type hash: $[hash])\n", hash->ptr);
	vv->type = etnTypeForHash(hash->ptr);
	if (NULL == vv->type) {
		status = StatusNotImplemented;
		goto done;
	}
	vv->data = malloc(vv->type->size);
	if (NULL == vv->data) {
		status = StatusNoMemory;
		goto done;
	}
	if (vv->type->kind >= EtnKindInvalid) {
		status = StatusFail;
		goto done;
	}
	status = decoderMap[vv->type->kind].decode (d, *vv);
	if (StatusOk != status) {
		goto done;
	}

	debugXPrint(typesDebug, "Decoded any type\n");

done:
	if (StatusOk != status && NULL != vv->data) {
		free (vv->data);
	}

	return status;
}

// Decode a union into EtnValue v.
static Status
_decodeUnion(EtnDecoder *d, EtnValue v)
{
	ASSERT (d);
	ASSERT (v.type);
	ASSERT (v.data);

	Status status;
	uint64_t unionId;
	EtnValue *vv = (EtnValue *)v.data;
	vv->data = NULL;

	_updateIndexToData(d, v);

	debugXPrint(typesDebug, "Decoding union type\n");
	status = decoderMap[EtnKindUnionId].decode (d, EtnToValue(&UnionIdType, &unionId));
	if (StatusOk != status) {
		goto done;
	}
	if (unionId >= ((EtnUnionType *)v.type)->len) {
		status = StatusNotImplemented;
		goto done;
	}
	vv->type = ((EtnUnionType *)v.type)->fields[unionId].elem;
	ASSERT (vv->type);
	vv->data = malloc(vv->type->size);
	if (NULL == vv->data) {
		status = StatusNoMemory;
		goto done;
	}
	if (vv->type->kind >= EtnKindInvalid) {
		status = StatusFail;
		goto done;
	}
	status = decoderMap[vv->type->kind].decode (d, *vv);
	if (StatusOk != status) {
		goto done;
	}

	debugXPrint(typesDebug, "Decoded union type\n");

done:
	if (StatusOk != status && NULL != vv->data) {
		free (vv->data);
	}

	return status;
}

static Status
_decodeNotImplemented(EtnDecoder *d, EtnValue v)
{
	ASSERT (d);
	ASSERT (v.type);
	ASSERT (v.data);

	_updateIndexToData(d, v);

	return StatusNotImplemented;
}

// Decode an arbitrary type into EtnValue v.
Status
etnDecode(EtnDecoder *d, EtnValue v)
{
    ASSERT (d);
    ASSERT (v.type);
    ASSERT (v.data);

    // Initialy use static buffer; only malloc if it needed to grow.
    static void *indexToData[MinIndexToDataLength * sizeof (void *)];
    if (d->indexToData == NULL) {
	d->indexToData = indexToData;
	d->indexToDataLength = MinIndexToDataLength;
    }

    // NOTE: No need to set d->indexToData[...] to zero.
    d->nextIndex = 0;

    if (v.type->kind >= EtnKindInvalid) {
	return StatusFail;
    }

    return decoderMap[v.type->kind].decode (d, v);
}

// A read function that reads from memory.
static int
etnBufferDecoderRead(EtnDecoder * _d, uint8_t *data, EtnLength length)
{
    ASSERT(_d);
    ASSERT(data || ! length);

    EtnBufferDecoder *d = (EtnBufferDecoder *) _d;

    if (d->dataCurrent + length > d->dataEnd) {
	return 0;
    }

    // FIXME: Copied from encoder.c.
    // Try to take advantage of word-sized copies;
    // benchmarks indicates this helps.
    EtnLength remaining = length;
    if (remaining % sizeof(long) == 0)
        {
            for (; remaining >= sizeof(long); remaining -= sizeof(long))
                {
                    *(long *) data = *(long *) d->dataCurrent;
                    d->dataCurrent += sizeof(long);
                    data += sizeof(long);
                }
        }

    if (remaining)
        {
            memcpy(data, d->dataCurrent, remaining);
            d->dataCurrent += remaining;
        }

    return length;
}

// Generic reset; should only be called by subclasses.
void
etnDecoderReset(EtnDecoder *d, int (*read) (EtnDecoder *d, uint8_t *data, EtnLength length))
{
    ASSERT(d);
    if (d->indexToData)
	{
	    // Initialy uses static buffer; only malloc'ed if it needs to grow.
	    if (d->indexToDataLength > MinIndexToDataLength)
		{
		    free (d->indexToData);
		}
	    d->indexToData = NULL;
	}
    d->indexToDataLength = 0;
    d->nextIndex         = 0;
    d->read              = read;
}

// Generic free; should only be called by subclasses.
void
etnDecoderFree(EtnDecoder *d)
{
    ASSERT(d);
    // Initialy uses static buffer; only malloc'ed if it needs to grow.
    if (d->indexToData && d->indexToDataLength > MinIndexToDataLength)
	{
	    free (d->indexToData);
	}
    free (d);
}

// Reset buffer decoder and use a newly provided buffer.
void
etnBufferDecoderReset(EtnBufferDecoder *d, uint8_t *data, EtnLength length)
{
    ASSERT(d);
    etnDecoderReset ((EtnDecoder *) d, etnBufferDecoderRead);
    d->dataCurrent = data;
    d->dataEnd     = data + length;
}

// Free buffer decoder.
void
etnBufferDecoderFree(EtnBufferDecoder *d)
{
    ASSERT(d);
    etnDecoderFree((EtnDecoder *) d);
}

// Create a new buffer decoder.
EtnBufferDecoder *
etnBufferDecoderNew(uint8_t *data, EtnLength length)
{
    EtnBufferDecoder *d = malloc (sizeof (EtnBufferDecoder));
    ASSERT(d);
    memzero(d, sizeof (EtnBufferDecoder)); // This must be done before Reset.
    etnBufferDecoderReset(d, data, length);
    return d;
}

// Reset path decoder and use a newly provided path.
void
etnPathDecoderReset(EtnPathDecoder *d, char *path)
{
    ASSERT(d);
    etnDecoderReset ((EtnDecoder *) d, etnBufferDecoderRead); // Reuse buffer decoder read.

    if (d->dataOriginal)
	{
	    free (d->dataOriginal);
	}

    if (path != NULL)
	{
	    Slice data;
	    mixinReadVar (&data, path);
	    d->dataOriginal = d->bufferDecoder.dataCurrent = data.array;
	    d->bufferDecoder.dataEnd = data.array + data.len;
	}
    else
	{
	    d->dataOriginal = d->bufferDecoder.dataCurrent = NULL;
	    d->bufferDecoder.dataEnd = NULL;
	}
}

// Create a new path decoder.
EtnPathDecoder *
etnPathDecoderNew(char *path)
{
    EtnPathDecoder *d = malloc (sizeof (EtnPathDecoder));
    ASSERT(d);
    memzero(d, sizeof (EtnPathDecoder)); // This must be done before Reset.
    etnPathDecoderReset(d, path);
    return d;
}
