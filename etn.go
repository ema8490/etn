package etn

import (
	"ethos/types"
	"math"
	"io"
	"os"
	"reflect"
)

const (
	bufsize = 512
	pNIL = 0
	pIDX = 1
	pVAL = 2
)

type Error string
func (e Error) String() string {
	return string(e)
}

/* Keeps trace of the type and number of addressable elements (of slices, arrays, structs and referenced pointers) */
type vmap struct {
	index int // Number of addressable elements so far
	t reflect.Type // Type of the current addressable element
}

/* Represents the Encoder */
type Encoder struct {
	w io.Writer // Writer that writes the encoded data into the buffer buf
	addrToIndex map[uintptr]vmap // Keeps trace of the type and number of addressable elements
	index int // Keeps trace of the number of encoded elements
	buf []byte // Stores encoded elements
}

/* Represents the Decoder */
type Decoder struct {
	r io.Reader // Reader that reads the decoded data from the buffer buf
	indexToValue []reflect.Value // Keeps trace of the decoded values
	index int // USELESS?
	buf []byte // Stores encoded elements
}


type Value struct {
	Data interface{}
	*types.Type
}

type Field struct {
	Name string
	*Value
}

var tt = newTypeTable()

func init() {
	for _, v := range builtins {
		Register(v.typ, v.hash)
	}
}

/* Map the concrete type of the value contained in the empty interface  to the hash contained in the byte slice. */
func Register(i interface{}, h types.Hash) bool {
	return tt.Register(reflect.TypeOf(i), h)
}

/* NewEncoder returns an Encoder that writes to the io.Writer w */
func NewEncoder(w io.Writer) *Encoder {
	return &Encoder{w:w, buf:make([]byte, bufsize)}
}

/* NewDecoder returns an Decoder that reads from the io.Reader r */
func NewDecoder(r io.Reader) *Decoder {
	return &Decoder{r:r, buf:make([]byte, bufsize)}
}

/* Encode sends the data item contained in the empty interface value. */
func (e *Encoder) Encode(i interface{}) os.Error {
	//return e.EncodeValue(reflect.Indirect(reflect.ValueOf(i)))
	return e.EncodeValue(reflect.ValueOf(i))
}

/* Decode reads a value with the concrete type of the value contained in the
 empty interface and stores it into said value. The value contained in the
 empty interface must be a pointer to a value of the desired type. */
func (d *Decoder) Decode(i interface{}) os.Error {
	return d.DecodeValue(reflect.Indirect(reflect.ValueOf(i)))
}

/* Writes encoded data into buffer buf */
func (e *Encoder) write(b []byte) {
	if len(b) == 0 { return }
	if _, err := e.w.Write(b); err != nil {
		panic(Error(err.String()))
	}
}

/* Reads encoded data from buffer buf */
func (d *Decoder) read(b []byte) {
	if len(b) == 0 { return }
	if _, err := d.r.Read(b); err != nil {
		panic(Error(err.String()))
	}
}

/* Encodes a uint8 in little endian and writes it into the Encoder buffer buf */
func (e *Encoder) uint8(u uint8) {
	e.write([]byte{u})
}

/* Decodes a uint8 and reads it from the Decoder buffer buf */
func (d *Decoder) uint8() (v uint8) {
	d.read(d.buf[:1])
	return uint8(d.buf[0])
}

/* Encodes a uint16 in little endian and writes it into the Encoder buffer buf */
func (e *Encoder) uint16(u uint16) {
	e.write([]byte{byte(u), byte(u >> 8)})
}

/* Decodes a uint16 and reads it from the Decoder buffer buf */
func (d *Decoder) uint16() (v uint16) {
	d.read(d.buf[:2])
	return uint16(d.buf[0]) | uint16(d.buf[1]) << 8
}

/* Encodes a uint32 in little endian and writes it into the Encoder buffer buf */
func (e *Encoder) uint32(u uint32) {
	e.write([]byte{byte(u), byte(u >> 8), byte(u >> 16), byte(u >> 24)})
}

/* Decodes a uint32 and reads it from the Decoder buffer buf */
func (d *Decoder) uint32() (v uint32) {
	d.read(d.buf[:4])
	return uint32(d.buf[0]) | uint32(d.buf[1]) << 8 | uint32(d.buf[2]) << 16 | uint32(d.buf[3]) << 24
}

/* Encodes a uint64 in little endian and writes it into the Encoder buffer buf */
func (e *Encoder) uint64(u uint64) {
	e.write([]byte{byte(u), byte(u >> 8), byte(u >> 16), byte(u >> 24),
		byte(u >> 32), byte(u >> 40), byte(u >> 48), byte(u >> 56)})
}

/* Decodes a uint64 and reads it from the Decoder buffer buf */
func (d *Decoder) uint64() (v uint64) {
	d.read(d.buf[:8])
	return uint64(d.buf[0]) | uint64(d.buf[1]) << 8 | uint64(d.buf[2]) << 16 | uint64(d.buf[3]) << 24 |
		uint64(d.buf[4]) << 32 | uint64(d.buf[5]) << 40 | uint64(d.buf[6]) << 48 | uint64(d.buf[7]) << 56
}

/* Encodes a string in byte writing it into the Encoder buffer buf */
func (e *Encoder) string(v string) {
	e.length(uint32(len(v)))
	e.write([]byte(v))
}

/* Decodes a string reading it from the Decoder buffer buf */
func (d *Decoder) string() (v string) {
	b := d.buf[:d.length()]
	d.read(b)
	return string(b)
}

// encode/decode length using base-128. If >1 digit is encoded,
// prefix the encoded value with its length, negated.
func (e *Encoder) length(l uint32) {
	e.uint32(l)
}

func (d *Decoder) length() (uint32) {
	return d.uint32()
}

/* Called by Encode(), takes a reflect.Value. */
func (e *Encoder) EncodeValue(v reflect.Value) (err os.Error) {
	defer func() {
		if x := recover(); x != nil {
			switch x := x.(type) {
			case Error:
				err = os.Error(x)
			default:
				panic(x)
			}
		}
	}()

	e.addrToIndex = make(map[uintptr]vmap)
	e.index = 0
	e.encode(v)
	return
}

/* Checks the value of the passed data v and encodes it into buffer b */
func (e *Encoder) encode(v reflect.Value) {
	if v.CanAddr() { // Need to figure out exactly what can and can't be addressed
		if _, ok := e.addrToIndex[v.Addr().Pointer()]; !ok {
			e.addrToIndex[v.Addr().Pointer()] = vmap{index:e.index, t:v.Type()}
		}
	}
	e.index++

	switch v.Kind() {
	case reflect.Bool:
		if v.Bool() {
			e.uint8(1)
		} else {
			e.uint8(0)
		}

	case reflect.Uint64:
		e.uint64(v.Uint())
	case reflect.Uint32:
		e.uint32(uint32(v.Uint()))
	case reflect.Uint16:
		e.uint16(uint16(v.Uint()))
	case reflect.Uint8:
		e.uint8(uint8(v.Uint()))

	case reflect.Int64:
		e.uint64(uint64(v.Int()))
	case reflect.Int32:
		e.uint32(uint32(v.Int()))
	case reflect.Int16:
		e.uint16(uint16(v.Int()))
	case reflect.Int8:
		e.uint8(uint8(v.Int()))

	case reflect.Float32:
		e.uint32(math.Float32bits(float32(v.Float())))
	case reflect.Float64:
		e.uint64(math.Float64bits(float64(v.Float())))

	case reflect.String:
		e.string(v.String())

	case reflect.Slice:
		l := v.Len()
		e.length(uint32(l))
		for i := 0; i < l; i++ {
			e.encode(v.Index(i))
		}

	case reflect.Array:
		l := v.Len()
		e.length(uint32(l))
		for i := 0; i < l; i++ {
			e.encode(v.Index(i))
		}

	case reflect.Ptr:
		if v.IsNil() {
			e.uint8(pNIL)
			return
		}
		el := v.Elem()
		if i, ok := e.addrToIndex[el.Addr().Pointer()]; ok {
			e.uint8(pIDX)
			if i.t != el.Type() { // now pointing to first element in object
			        // e.g.,  A struct { B int32 } then A and B have different types
				e.length(uint32(i.index+1))
			} else { // points to object
				e.length(uint32(i.index))
			}
		} else {
			e.uint8(pVAL)
			e.encode(el)
		}

	case reflect.Struct:
		l := v.NumField()
		for i := 0; i < l; i++ {
			e.encode(v.Field(i))
		}

	case reflect.Map:
		e.length(uint32(v.Len()))
		for _, k := range v.MapKeys() {
			e.encode(k)
			e.encode(v.MapIndex(k))
		}

	case reflect.Interface:
		el := v.Elem()
		if !el.IsValid() {
			e.encode(reflect.Indirect(reflect.ValueOf([]byte{})))
			return
		}
		h := tt.HashForType(el.Type())
		if h == nil {
			panic(os.NewError("Unregistered type"))
		}
		e.encode(reflect.Indirect(reflect.ValueOf(h)))
		e.encode(el)

	default:
		panic(os.NewError("Unsupported type"))
	}
}

/* Called by Decode(), takes a reflect.Value. */
func (d *Decoder) DecodeValue(v reflect.Value) (e os.Error) {
	defer func() {
		if x := recover(); x != nil {
			switch x := x.(type) {
			case Error:
				e = os.Error(x)
			default:
				panic(x)
			}
		}
	}()

	d.indexToValue = make([]reflect.Value, 0, bufsize)
	d.index = 0
	d.decode(v)
	return
}

/* Checks the value of the passed data v and encodes it into buffer b */
func (d *Decoder) decode(v reflect.Value) {
	d.indexToValue = append(d.indexToValue, v)

	switch v.Kind() {
	case reflect.Bool:
		v.SetBool(d.uint8() == 1)

	case reflect.Uint8:
		v.SetUint(uint64(d.uint8()))
	case reflect.Uint16:
		v.SetUint(uint64(d.uint16()))
	case reflect.Uint32:
		v.SetUint(uint64(d.uint32()))
	case reflect.Uint64:
		v.SetUint(d.uint64())

	case reflect.Int8:
		v.SetInt(int64(d.uint8()))
	case reflect.Int16:
		v.SetInt(int64(d.uint16()))
	case reflect.Int32:
		v.SetInt(int64(d.uint32()))
	case reflect.Int64:
		v.SetInt(int64(d.uint64()))

	case reflect.Float32:
		v.SetFloat(float64(math.Float32frombits(d.uint32())))
	case reflect.Float64:
		v.SetFloat(math.Float64frombits(d.uint64()))

	case reflect.String:
		v.SetString(d.string())

	case reflect.Slice:
		l := int(d.length())
		if l == 0 {
			v.Set(reflect.Zero(v.Type()))
			return
		}
		v.Set(reflect.MakeSlice(v.Type(), l, l))
		for i := 0; i < l; i++ {
			d.decode(v.Index(i))
		}

	case reflect.Array:
		l := int(d.length())
		if l != v.Len() {
			panic(os.NewError("Size mismatch"))
		}
		for i := 0; i < l; i++ {
			d.decode(v.Index(i))
		}

	case reflect.Ptr:
		switch d.uint8() {
		case pNIL:
			v.Set(reflect.Zero(v.Type()))
		case pIDX:
			v.Set(d.indexToValue[d.length()].Addr())
		case pVAL:
			e := reflect.Indirect(reflect.New(v.Type().Elem()))
			d.decode(e)
			v.Set(e.Addr())
		default:
			panic(os.NewError("Bad pointer type"))
		}

	case reflect.Struct:
		l := v.NumField()
		for i := 0; i < l; i++ {
			d.decode(v.Field(i))
		}

	case reflect.Map:
		kt, et := v.Type().Key(), v.Type().Elem()
		l := d.length()
		if l == 0 {
			v.Set(reflect.Zero(v.Type()))
			return
		}
		v.Set(reflect.MakeMap(v.Type()))
		for ; l > 0; l-- {
			k, el := reflect.Indirect(reflect.New(kt)), reflect.Indirect(reflect.New(et))
			d.decode(k)
			d.decode(el)
			v.SetMapIndex(k, el)
		}

	case reflect.Interface:
		var h types.Hash
		d.decode(reflect.Indirect(reflect.ValueOf(&h)))
		if h == nil {
			return
		}
		t := tt.TypeForHash(h)
		if t == nil {
			println(h, len(h))
			panic(os.NewError("Unregistered type"))
		}
		el := reflect.Indirect(reflect.New(t))
		d.decode(el)
		v.Set(el)

	default:
		panic(os.NewError("Unsupported type"))
	}
}
