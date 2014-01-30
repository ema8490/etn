package etn

import (
	"os"
	"reflect"
	"sync"
	"unicode"
)

// The type id is simply a uint64
type id uint64

// Type header contains a uint64 Call and an id (that is a in fact a uint64)
type header struct {
	Id id
	Call uint64
}

// Type Call, different from Call in type header, contains two interfaces, an os.Error and a boolean Channel
type Call struct {
	Args  interface{}
	Reply interface{}
	Error os.Error
	Done chan bool
}

// Type callId contains an id and a Mutex
// Mutex is used to lock a callId variable when it has to be modified
type callId struct {
	id id
	sync.Mutex
}
var cid callId

// Function of callId type
// The function locks variable c, increments its id, returns the previous id and eventually unlock c 
func (c *callId) Increment() (i id) {
	c.Lock()
	i = c.id
	c.id++
	c.Unlock()
	return
}

// Type Server contains a reflect.Value, an array of int, an Encoder and a Decoder
// v is the reflect.Value of an empty interface used during the creation of the Server
// s contains the indexes of the exported methods the Server is supposed to respond to
// Encoder and Decoder are used to transmit and receive values
type Server struct {
	v reflect.Value
	methods []int
	e *Encoder
	d *Decoder
}

// Type Client contains a map where the element type is *Call and the key type is int, an Encoder and a Decoder
// Encoder and Decoder are used to transmit and receive values
type Client struct {
	calls map[id]*Call
	e *Encoder
	d *Decoder
}

// Create a client that transmits and receives values using the
// supplied Encoder and Decoder.
func NewClient(e *Encoder, d *Decoder) *Client {
	if e == nil {
		return nil
	}
	return &Client{make(map[id]*Call), e, d}
}

// Create a server using the value stored in the empty interface that
// transmits and receives values using the supplied Encoder and Decoder.
// The newly-created server will respond to the exported methods of this
// value.
func NewServer(v interface{}, e *Encoder, d *Decoder) (s *Server) {
	if d == nil {
		return nil
	}
	s = &Server{reflect.ValueOf(v), nil, e, d}

	for i, t := 0, s.v.Type(); i < t.NumMethod(); i++ {
		if unicode.IsUpper(int(t.Method(i).Name[0])) {
			s.methods = append(s.methods, i)
		}
	}
	return
}

// Function of Client type
// Collect, decode & dispatch replies
func (c *Client) Dispatch() bool {
	if len(c.calls) == 0 {
		return false
	}

	id := id(0)
	c.d.Decode(&id)
	if call, ok := c.calls[id]; ok {
		call.Error = c.d.Decode(call.Reply)
		c.calls[id] = nil, false	//it assigns nil to c.calls[id].Error and false to c.calls[id].Done
		call.Done <- true
		return true
	}
	return false
}

// Function of Client type
// it creates a new call using the parameters
// if either call.reply or c.d (Client's decoder) is nil, the call is done
// otherwise, the call is added to the Client's calls map
func (c *Client) Go(id uint64, args, reply interface{}) (call *Call) {
	// Lock client?
	var err os.Error
	cid := (&cid).Increment()
	if err = c.e.Encode(header{cid, id}); err != nil {
		return &Call{Error:err}
	}
	if args != nil {
		if err = c.e.Encode(args); err != nil {
			return &Call{Error:err}
		}
	}
	call = &Call{args, reply, nil, make(chan bool, 1)}
	if reply == nil || c.d == nil {
		call.Done <- true
	} else {
		c.calls[cid] = call
	}
	return
}

// Function of Client type
// Synchronous: does not return until call is finished.
func (c *Client) Call(id uint64, args, reply interface{}) (err os.Error) {
	// XXX: Suspend, call dispatcher
	call := c.Go(id, args, reply)
	if call.Error != nil {
		return call.Error
	}
	loop: for {
		select {
		case _ = <-call.Done:
			break loop
		default:
			c.Dispatch()
		}
	}
	return
}

// Function of Server type
// Server handles the request
func (s *Server) Handle() os.Error {
	var r []reflect.Value

	c := header{}
	if err := s.d.Decode(&c); err != nil {
		return err
	}

	f := s.v.Method(s.methods[int(c.Call)]) // Will panic on oor
	t := f.Type()
	n := t.NumIn()

	p := make([]reflect.Value, n)
	for i := 0; i < n; i++ {
		p[i] = reflect.Indirect(reflect.New(t.In(i)))
		s.d.DecodeValue(p[i])
	}

	if t.IsVariadic() {
		r = f.CallSlice(p)
	} else {
		r = f.Call(p)
	}

	if l := len(r); l > 0 && s.e != nil {
		s.e.EncodeValue(reflect.ValueOf(c.Id))
		for i := 0; i < l; i++ {
			s.e.EncodeValue(r[i])
		}
	}
	return nil
}
