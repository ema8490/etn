package etn

//Type trie is used to build a tree
//It contains a byte (used as index) and interface and two pointers to trie type (one to the child and one to the sibling)
type trie struct {
	index byte
	value interface{}
	down, next *trie //down: child, next: sibling
}

// Function of trie type
// Find the longest present prefix of b
// it returns an array of bytes and pointer to a trie variable
// This trie variable is the last node whose index matches the prefix 
// or it is t if there is no matching index in t's children.
func (t *trie) prefix(b []byte) (s []byte, r *trie) {
	for r, t = t, t.down; len(b) > 0 && t != nil; {
		if t.index == b[0] {
			r, t, b = t, t.down, b[1:]
		} else {
			t = t.next
		}
	}
	return b, r
}

// Function of trie type
// It looks up for a key in t variable
// If the key doesn't exist or the interface returned by prefix function is nil, it returns nil (interface) and false (bool)
// otherwise, it returns the inferface and true
func (t *trie) Lookup(key []byte) (value interface{}, ok bool) {
	key, r := t.prefix(key)
	if len(key) > 0 || r.value == nil {
		return nil, false
	}
	return r.value, true
}

// Function of trie type
// It inserts a new interface using the key received as parameter
func (t *trie) Insert(key []byte, value interface{}) {
	key, r := t.prefix(key)
	if len(key) == 0 { // Key exists
		r.value = value
	} else { // Need to insert key
		for len(key) > 0 {
			r.down = &trie{index:key[0], next:r.down}
			r, key = r.down, key[1:]
		}
		r.value = value
	}
}

// Function of trie type
// It deletes an interface by setting it to nil according to the key received as parameter 
// Search, record deepest node w/ multiple children, siblings
func (t *trie) Delete(key []byte) {	
	l, r, v := (**trie)(nil), (*trie)(nil), (*trie)(nil)
	for t = t.down; len(key) > 0 && t != nil; {
		if t.index == key[0] {
			if t.down != nil && t.down.next != nil {
				l, r = &t.down, t.down.next
			}
			v, t, key = t, t.down, key[1:]
		} else {
			if t.next != nil {
				l, r = &t.next, t.next.next
			}
			t = t.next
		}
	}
	if len(key) > 0 {
		return
	}
	if l != nil {
		*l = r // Chop off children, let GC clean up
	}
	v.value = nil
}
