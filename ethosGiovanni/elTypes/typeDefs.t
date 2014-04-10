ElString string
ElBool bool
ElInt int64
ElFloat float64

ElUInt8 uint8
ElUInt16 uint16
ElUInt32 uint32
ElUInt64 uint64

ElInt8 int8
ElInt16 int16
ElInt32 int32
ElInt64 int64

ElFloat32 float32
ElFloat64 float64

ElTuple struct {
	Content []Any
	Type ElInt
}

TestPrimitive string

TestComposite struct {
	A string
	B string
}

TestTuple []string

TestMixed struct {
	ABool bool
	ATuple TestTuple
}
