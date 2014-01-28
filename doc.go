/*
Package etn manages the encoding and decoding of values for interprocess communication
and provides access to the methods of an object exported over an IPC channel. etn and its
encoding are canonical for IPC in Ethos, but are platform-independent by design.

To transmit values, create an Encoder and present it with a series of data items as values or
addresses that can be derferenced to values. On the receiving end, create a Decoder and
present it with pointers to variables of the desired type that will hold the decoded
values. Circular values are fully supported.

To perform remote procedure calls, create a Client. The Client has three methods, Call, Go,
and Dispatch. Call is synchronous; Go is asynchronous (i.e. Call waits for the remote call
to complete before returning, Go does not). Dispatch collects and decodes return values
and dispatches them to waiting calls.

To serve remote procedure calls, create a Server. The value used to create a Server specifies
the method set that the server will respond to and contains any required internal state.
Once a Server has been created, call its Handle method to decode and perform methods.

etn types are described below. Note that although the types are likely final, the
encoding of a given type may change.

bool: represents the truth values true and false. Encoded within a byte: 1 for true, 0 for false.
Represented in Go by type bool.

Integer types: u?int(8|16|32|64). Each represents the set of n-bit signed/unsigned integers.
Encoded in n-bit little-endian; an n-bit type is always encoded using n bits. Represented in
Go by types u?int(8|16|32|64).

Floating point types: float(32|64). Each represents the set of n-bit floating point numbers.
Encoded in little-endian IEE-754. Represented in Go by types float(32|64).

string: represents the set of Unicode strings. Encoded using UTF-8. The length in bytes 
l of the UTF-8 encoded string is sent first as a uint32, followed by the l bytes themselves.
Represented in Go by type string.

tuple: an ordered list of elements of a given type. The length in elements l of a tuple
value is sent as a uint32, followed by l encoded values of the tuple's element type.
Represented in Go by array and slice types.

pointer: a reference to a value of a given type. The way a pointer value is classified and encoded
depends on whether or not the pointer is nil and whether it has already been encoded as part
of a larger value (e.g. as part of a circular value). If a pointer value is nil, PNIL is
encoded within a byte and encoding is complete. If a pointer value has been encoded before as
part of the same, larger value, PIDX is encoded within a byte, followed by the index of the
previously encoded value. Otherwise, PVAL is encoded within a byte, followed by the encoded value
that the pointer references. Represented in Go by pointer types.

struct: a composite type containing named fields with specified types. Names are ignored when
sending or receiving structs; a struct is sent by encoding each field in the order specified by
the type of the structure. Represented in Go by struct types. In order to encode or decode a
struct, all fields must be exported.

dict: a map of keys to values. Not currently implemented.

smart pointer: a reference to a value which may or may not be in memory. Not currently implemented.

any: a reference to a value of any type. Sent by encoding the hash for the type of the concrete
value references as a Tuple, followed by the encoded value that the any references. Represented
in Go by interface types. In order to encode or decode a value contained in an interface,
the hash for the value's type must have been registered.

etn cannot encode the following Go types:
complex64 complex128 uint int uintptr

Here is a simple example using Ethos IPC:

	pacakge server

	const (
		MHello = iota
	)

	type (
		HelloArgs struct { Name string }
		HelloServer int
	)

	func (HelloServer) Hello(name string) string {
		return "Hello, " + name
	}

The server:

	package main

	import (
		"ethos/etn"
		"ethos/syscall"
		"os"
		"./server"
	)

	func main() {
		_, lfd, s := syscall.Synch(syscall.Advertise(syscall.CurrentDirectory, "hello"))
		if s != syscall.StatusOk {
			os.Exit(1)
		}
		_, ifd, s := syscall.Synch(syscall.Import(lfd))
		if s != syscall.StatusOk {
			os.Exit(1)
		}

		e, d := etn.NewEncoder(os.NewFile(int(ifd), "")), etn.NewDecoder(os.NewFile(int(ifd), ""))
		hello := etn.NewServer(server.HelloServer(0), e, d)

		// Handle one call and exit
		hello.Handle()
	}

The client:

	package main

	import (
		"ethos/etn"
		"ethos/syscall"
		"os"
		"./server"
	)

	func main() {
		var h

		_, ifd, s := syscall.Synch(syscall.Ipc(syscall.CurrentDirectory, "hello", "", nil))
		if s != syscall.StatusOk {
			os.Exit(1)
		}

		e, d := etn.NewEncoder(os.NewFile(int(ifd), "")), etn.NewDecoder(os.NewFile(int(ifd), ""))
		c := etn.NewClient(e, d)
		c.Call(server.MHello, server.HelloArgs{"world"}, &h)
		println(h)
	}
*/
package etn
