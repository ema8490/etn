// FIXME: This will create Uint32TupleType with NoName as an alias.
// I really just want Uint32TupleType, but explicitly using that
// name here will cause a conflict. Arrays work differently; see
// TestArray.t.
NoName []uint32

TupleTuple []NoName

StringAndAnyTuple struct{
      Field1 string
      Field2 []Any
}
