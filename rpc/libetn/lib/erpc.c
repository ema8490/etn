#include <ethos/dual/core.h>
#include <ethos/generic/assert.h>
#include <ethos/generic/debug.h>
#include <ethos/generic/etn.h>
#include <stdint.h>

#define arrayLen(x) ((sizeof(x)/sizeof(x[0])))

struct EtnRpcHost {
	EtnValue v;
	EtnEncoder *e;
	EtnDecoder *d;
};

EtnRpcHost *
etnRpcHostNew(EtnValue v, EtnEncoder *e, EtnDecoder *d) 
{
    EtnRpcHost *h;
    if (!(h = (EtnRpcHost *) malloc(sizeof(EtnRpcHost)))) 
	{
	    return NULL;
	}
    *h = (EtnRpcHost) {v, e, d};
    return h;
}

// FIXME: Somewhat of a kludge to support pulling 
//        a connection out of an RpcHost. In the Ethos
//        kernel, a connection is tied to a PacketEncoder.
//        See also use in kernel.
EtnEncoder *
etnRpcHostGetEncoder (EtnRpcHost *host)
{
    return host->e;
}

Status
etnRpcCall(EtnRpcHost *h, EtnCallId call, EtnValue args, EtnLength *length) 
{
    Status status = StatusOk;
    EtnLength _length = *length = 0;

    if (args.data)
	{
	    // NOTE: The Ethos kernel sends a packet on flush;
	    //       therefore, we only want to flush once per
	    //       RPC encode.
	    debugXPrint(typesDebug, "Calling method $[ulong] with arguments\n", call);
	    status = etnEncodeWithoutFlush(h->e, EtnToValue(&CallIdType, &call), &_length);
	    if (StatusOk != status)
		{
		    return status;
		}
	    *length += _length;
	    status = etnEncode(h->e, args, &_length);
	    if (StatusOk != status)
		{
		    return status;
		}
	    *length += _length;
	}
    else
	{
	    debugXPrint(typesDebug, "Calling method $[ulong] without arguments\n", call);
	    status = etnEncode(h->e, EtnToValue(&CallIdType, &call), length);
	}

    return status;
}

Status
etnRpcHandle(EtnRpcHost *h) 
{
    Status status = StatusOk;
    EtnMethodType *mt;
    EtnValue p = { NULL, NULL };

    EtnCallId call;
    etnDecode(h->d, EtnToValue(&CallIdType, &call));
    if (call >= h->v.type->methods.num)
	{
	    status = StatusNotImplemented;
	    goto done;
	}

    debugXPrint(typesDebug, "Handling method $[ulong]\n", call);

    mt = &h->v.type->methods.list[call];
	
    // Receive args (should use Refs)
    if (mt->params != 0) 
	{
	    p.type = (EtnType *)mt->params;
	    p.data = (uint8_t *)malloc(p.type->size);
	    status = etnDecode(h->d, p);
	    if (StatusOk != status)
		{
		    goto done;
		}
	}

    // Call
    mt->call(h, p.data);

    // NOTE: RPC implementation must free components of decoded type.
done:
    if (p.data)
	{
	    free (p.data);
	}

    return status;
}
