package etn

import (
	"ethos/types"
	"reflect"
	"sync"
)

type typetable struct {
	hashToType *trie
	typeToHash map[reflect.Type]types.Hash
	sync.Mutex
}

// Singleton?
func newTypeTable() *typetable {
	return &typetable{hashToType:new(trie), typeToHash:make(map[reflect.Type]types.Hash)}
}

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

func (tt *typetable) TypeForHash(h types.Hash) reflect.Type {
	tt.Lock()
	t, ok := tt.hashToType.Lookup([]byte(h))
	tt.Unlock()
	if !ok {
		return nil
	}
	return t.(reflect.Type)
}

func (tt *typetable) HashForType(t reflect.Type) (h types.Hash) {
	tt.Lock()
	h, _ = tt.typeToHash[t]
	tt.Unlock()
	return
}
