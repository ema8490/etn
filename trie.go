package etn

type trie struct {
	index byte
	value interface{}
	down, next *trie
}

// Find the longest present prefix of b
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

func (t *trie) Lookup(key []byte) (value interface{}, ok bool) {
	key, r := t.prefix(key)
	if len(key) > 0 || r.value == nil {
		return nil, false
	}
	return r.value, true
}

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

func (t *trie) Delete(key []byte) {
	// Search, record deepest node w/ multiple children, siblings
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
