package t

import (
	"bytes"
	"ethos/types"
)

const (
	UNDEFINED = iota
	ANY
	INT
	FLOAT
	STRING
	BOOL
	INT8
	INT16
	INT32
	INT64
	UINT8
	UINT16
	UINT32
	UINT64
	FLOAT32
	FLOAT64
	TUPLE
	MAP
	SET
	FUNCTION
	PACKAGE
	OBJECT
)

var TYPES = map[int]string{
	UNDEFINED: "UNDEFINED",
	ANY:       "ANY",
	INT:       "INT",
	FLOAT:     "FLOAT",
	STRING:    "STRING",
	BOOL:      "BOOL",
	FUNCTION:  "FUNCTION",
	TUPLE:     "TUPLE",
	MAP:       "MAP",
	SET:       "SET",
	PACKAGE:   "PACKAGE",
	OBJECT:    "OBJECT",
}

var ETN_TYPES = map[int][]byte{}

var ETN_BASES = map[int]int{
	INT:     types.VINT64,
	INT64:   types.VINT64,
	INT32:   types.VINT32,
	INT16:   types.VINT16,
	INT8:    types.VINT8,
	UINT64:  types.VUINT64,
	UINT32:  types.VUINT32,
	UINT16:  types.VUINT16,
	UINT8:   types.VUINT8,
	FLOAT:   types.VFLOAT64,
	FLOAT32: types.VFLOAT32,
	FLOAT64: types.VFLOAT64,
	BOOL:    types.VBOOL,
	STRING:  types.VSTRING,
}

func PrimitiveTypeFromBase(base int) int {
	for typ, b := range ETN_BASES {
		if base == b {
			return typ
		}
	}

	return UNDEFINED
}

var ETN_NAMES = map[int]string{
	INT:    "ElInt",
	FLOAT:  "ElFloat",
	STRING: "ElString",
	BOOL:   "ElBool",
	MAP:    "ElMap",
	TUPLE:  "ElTuple",
}

type TupleRep struct {
	Type    int
	Content []interface{}
	Len     int
}

type SetRep struct {
	Type    int
	Content map[string]interface{}
}

type MapRep struct {
	Type    int
	Content map[string]interface{}
	Source  string // directory path, if map is backed by an OS directory
}

func Register(ty int, hash []byte) {
	ETN_TYPES[ty] = hash[:]
}

func TypeFromEtnHash(hash []byte) int {
	for ty, h := range ETN_TYPES {
		if bytes.Compare(h, hash) == 0 {
			return ty
		}
	}
	return UNDEFINED
}

func IsComposite(ty int) bool {
	return ty == MAP || ty == TUPLE || ty == SET
}
