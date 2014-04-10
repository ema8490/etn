package t

type Lattice struct {
	Type    int
	Parents []*Lattice
}

func NewLattice(ty int, parents ...*Lattice) *Lattice {
	l := new(Lattice)
	l.Type = ty
	l.Parents = parents
	return l
}

func (self *Lattice) isTop() bool {
	return len(self.Parents) == 0
}

func Element(ty int) *Lattice {
	return LATTICE_TYPES[ty]
}

var Top = NewLattice(ANY)

var String = NewLattice(STRING, Top)
var Bool = NewLattice(BOOL, Top)
var UInt64 = NewLattice(UINT64, Top)
var Int = NewLattice(INT, Float)
var Int64 = NewLattice(INT64, Top)
var Float = NewLattice(FLOAT, Top)
var Float64 = NewLattice(FLOAT64, Float)

var UInt32 = NewLattice(UINT32, UInt64, Int64)
var Int32 = NewLattice(INT32, Int64, Float64, Int)
var Float32 = NewLattice(FLOAT32, Float64)

var UInt16 = NewLattice(UINT16, UInt32, Int32)
var Int16 = NewLattice(INT16, Int32, Float32)

var UInt8 = NewLattice(UINT8, UInt16, Int16)
var Int8 = NewLattice(INT8, Int16)

var Map = NewLattice(MAP, Top)

var Set = NewLattice(MAP, Top)

var Tuple = NewLattice(MAP, Top)

var LATTICE_TYPES = map[int]*Lattice{
	ANY:     Top,
	STRING:  String,
	BOOL:    Bool,
	UINT64:  UInt64,
	INT:     Int,
	INT64:   Int64,
	FLOAT:   Float,
	FLOAT64: Float64,
	UINT32:  UInt32,
	INT32:   Int32,
	FLOAT32: Float32,
	UINT16:  UInt16,
	INT16:   Int16,
	UINT8:   UInt8,
	INT8:    Int8,
	MAP:     Map,
	SET:     Set,
	TUPLE:   Tuple,
}

// Typechecks all operands to a target or fails.
// Targets must be ordered from lowest to highest
// in the lattice, and valid targets
func TypeCheck(operands []int, targets []int) int {
	for i := 0; i < len(operands); i++ {
		if LATTICE_TYPES[operands[i]] == nil {
			return UNDEFINED
		}
	}

	for i := 0; i < len(targets); i++ {
		target := targets[i]
		// try to typecheck each operand to this target
		checks := true
		for j := 0; j < len(operands); j++ {
			op := operands[j]
			if !check(op, target) {
				checks = false
				break
			}
		}
		if checks {
			return target
		}
	}
	return UNDEFINED
}

// Typechecks a to target or fails
// target must be a valid lattice element
func check(a int, target int) bool {
	at := LATTICE_TYPES[a]
	targett := LATTICE_TYPES[target]

	// check if element for a exists
	if at == nil {
		return false
	}

	// check if a and target are same element
	if at.Type == targett.Type {
		return true
	}

	// recurse on each one of a's parents
	for i := 0; i < len(at.Parents); i += 1 {
		if check(at.Parents[i].Type, targett.Type) {
			return true
		}
	}

	return false
}
