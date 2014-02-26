#include <ethos/dual/core.h>
#include <ethos/generic/assert.h>
#include <ethos/generic/debug.h>
#include <ethos/generic/etn.h>
#include <stdint.h>

#define arrayLen(x) ((sizeof(x)/sizeof(x[0])))

/* Represents a Remote Procedure Call Host */
struct EtnRpcHost {
	EtnValue v; // value holded by the host (@see types.h)
	EtnEncoder *e; // host encoder (@see etn.h)
	EtnDecoder *d; // host decoder (@see etn.h)
};

/* Given a EtnValue, EtnEncoder and EtnDecoder creates and return an EtnRpcHost */
EtnRpcHost *
etnRpcHostNew(EtnValue v, EtnEncoder *e, EtnDecoder *d) 
{
    EtnRpcHost *h;
    if (!(h = (EtnRpcHost *) malloc(sizeof(EtnRpcHost)))) // allocates a chunk of memory of the size of EtnRpcHost 
	{
	    return NULL;
	}
    *h = (EtnRpcHost) {v, e, d}; // assigns the given EtnValue, EtnEncoder, EtnDecoder to the new EtnRpcHost
    return h; // returns the new EtnRpcHost
}

// FIXME: Somewhat of a kludge to support pulling 
//        a connection out of an RpcHost. In the Ethos
//        kernel, a connection is tied to a PacketEncoder.
//        See also use in kernel.
/* Given a EtnRpcHost, returns its EtnEncoder (used in kernel) */
EtnEncoder *
etnRpcHostGetEncoder (EtnRpcHost *host)
{
    return host->e;
}

/* Given EtnRpcHost (that contains the encoder to perform the rpc call), EtnCallId (that is the id of the called method), EtnValue (that are the arguments that the called method takes as input); does a rpc call and returns the Status of the call (StatusOk is returned if everything works) and the length of the encoded elements in the rpc (length of the encoded call id + length of the encoded arguments the method takes in input) */
Status
etnRpcCall(EtnRpcHost *h, EtnCallId call, EtnValue args, EtnLength *length) 
{
    Status status = StatusOk;
    EtnLength _length = *length = 0;

    if (args.data) // the called method has at least one argument
	{
	    // NOTE: The Ethos kernel sends a packet on flush;
	    //       therefore, we only want to flush once per
	    //       RPC encode.
	    debugXPrint(typesDebug, "Calling method $[ulong] with arguments\n", call);
	    status = etnEncodeWithoutFlush(h->e, EtnToValue(&CallIdType, &call), &_length); // encodes the call id, without performing the rpc (no flush() performed)
	    if (StatusOk != status) // something went wrong
		{
		    return status; // error status returned
		}
	    *length += _length; // length of encoded elements increased (of call id length)
	    status = etnEncode(h->e, args, &_length); // encodes the arguments of the called method and executes the rpc (flush() performed)
	    if (StatusOk != status) // something went wrong
		{
		    return status; // error status returned
		}
	    *length += _length; // length of encoded elements increased (of arguments length)
	}
    else // the called method does not have any argument
	{
	    debugXPrint(typesDebug, "Calling method $[ulong] without arguments\n", call);
	    status = etnEncode(h->e, EtnToValue(&CallIdType, &call), length); // encodes the call id and executes the rpc (flush() performed)
	}

    return status; // if everything is ok, StatusOk is returned
}

/* Given EtnRpcHost (that contains the EtnValue v and then the method, and its arguments, to which we want perform a call), does the rpc and return the status of it */
Status
etnRpcHandle(EtnRpcHost *h) 
{
    Status status = StatusOk;
    EtnMethodType *mt;
    EtnValue p = { NULL, NULL }; //sets: EtnType* type = NULL, void* data = NULL

    EtnCallId call;
    etnDecode(h->d, EtnToValue(&CallIdType, &call)); // decodes the call id of the method of the rpc call
    if (call >= h->v.type->methods.num) // call id not implemented
	{
	    status = StatusNotImplemented; // sets the return status to StatusNotImplemented 
	    goto done;
	}

    debugXPrint(typesDebug, "Handling method $[ulong]\n", call);

    mt = &h->v.type->methods.list[call]; // takes the corresponding arguments for the method of the rpc call
	
    // Receive args (should use Refs)
    if (mt->params != 0) // if the methods takes at least one argument in input
	{
	    p.type = (EtnType *)mt->params; // stores in EtnValue p the type of the arguments
	    p.data = (uint8_t *)malloc(p.type->size); // allocates the needed memory space to store the decoded arguments
	    status = etnDecode(h->d, p); // decodes the method arguments
	    if (StatusOk != status) // something went wrong
		{
		    goto done;
		}
	}

    // Call
    mt->call(h, p.data); // does the Remote Procedure Call to the method (in EtnHost h) with the decoded arguments (in void* p.data)

    // NOTE: RPC implementation must free components of decoded type.
done:
    if (p.data) // the method takes at least one argument in input
	{
	    free (p.data); // free the memory space occupied by the method arguments
	}

    return status; // return the rpc status
}
