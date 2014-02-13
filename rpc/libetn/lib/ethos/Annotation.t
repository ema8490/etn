// FIXME: Hash should be [64]byte, but types package uses []byte.
//        et2g uses the types package, so this is the encoding actually
//        found it type graphs.
Annotation struct {
        Hash []byte
	Data string
	Base byte
}
