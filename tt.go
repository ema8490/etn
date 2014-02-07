package etn

import (
	"ethos/types"
	"reflect"
	"sync"
)

/* The typetable struct contains information for mapping the concrete type of the value to the hash */
type typetable struct {
	hashToType *trie
	typeToHash map[reflect.Type]types.Hash
	sync.Mutex
}

/* Creates a new typetable (constructor) */
func newTypeTable() *typetable {
	return &typetable{hashToType:new(trie), typeToHash:make(map[reflect.Type]types.Hash)}
}

/* Maps the concrete type of the value contained in the empty interface to the hash contained in the byte slice, and inserts it into the *trie and map[reflect.Type]types.Hash data structures. */
func (tt *typetable) Register(t reflect.Type, h types.Hash) (ok bool) {
	if _, ok := tt.typeToHash[t]; ok {
		return false
	}

	tt.Lock()
	tt.typeToHash[t] = h
	tt.hashToType.Insert([]byte(h), t)
	tt.Unlock()
	return true
}

/* Given an hash returns the corresponding type */
func (tt *typetable) TypeForHash(h types.Hash) reflect.Type {
	tt.Lock()
	t, ok := tt.hashToType.Lookup([]byte(h))
	tt.Unlock()
	if !ok {
		return nil
	}
	return t.(reflect.Type)
}

/* Given a type returns the corresponding hash value */
func (tt *typetable) HashForType(t reflect.Type) (h types.Hash) {
	tt.Lock()
	h, _ = tt.typeToHash[t]
	tt.Unlock()
	return
}
