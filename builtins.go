package etn

import "ethos/types"

type typePair struct {
	typ interface{}
	hash types.Hash
}

var builtins = [...]typePair{
	{int8(0), types.Builtins[types.VINT8].Hash},
	{uint8(0), types.Builtins[types.VUINT8].Hash},
	{int16(0), types.Builtins[types.VINT16].Hash},
	{uint16(0), types.Builtins[types.VUINT16].Hash},
	{int32(0), types.Builtins[types.VINT32].Hash},
	{uint32(0), types.Builtins[types.VUINT32].Hash},
	{int64(0), types.Builtins[types.VINT64].Hash},
	{uint64(0), types.Builtins[types.VUINT64].Hash},
	{float32(0), types.Builtins[types.VFLOAT32].Hash},
	{float64(0), types.Builtins[types.VFLOAT64].Hash},
	{true, types.Builtins[types.VBOOL].Hash},
	{"", types.Builtins[types.VSTRING].Hash},
	{interface{}(0), types.Builtins[types.VANY].Hash}}
