// FIXME: Hash should be [64]byte, but types package uses []byte.
//        et2g uses the types package, so this is the encoding actually
//        found in type graphs. If this is unified, this should also
//        be renamed "HashValue."
TGHashValue []byte

TypeNode struct {
	Hash TGHashValue
	Name string
	Base byte
	Annot *Annotation
	Size uint32
	Elems []TGHashValue
	Fields []string
}
