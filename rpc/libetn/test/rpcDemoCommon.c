#include <ethos/generic/assert.h>
#include <ethos/generic/fmt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "testGraph.h"
#include "unitTest.h"

void rpcTestHello (EtnRpcHost *h, uint64 cid)
{
	printf ("Executing RPC: Hello, world!\n");
}

void rpcTestSprintf (EtnRpcHost *h, uint64 cid, String *msg)
{
	xprint ("Executing RPC: $[string]\n", msg);
	String *control = stringAllocateInitialize ("Hello, parameterized world!");
	ASSERT (! stringCompare (msg, control));
	EtnLength length;
	rpcTestSprintfReplyCall (h, cid, msg, &length);
}

void rpcTestTwoArgsOneReturn (EtnRpcHost *h, uint64 cid, String *one, String *two)
{
	xprint ("Executing RPC: $[string] $[string]\n", one, two);
	String *controlOne = stringAllocateInitialize ("One");
	String *controlTwo = stringAllocateInitialize ("Two");
	ASSERT (! stringCompare (one, controlOne));
	ASSERT (! stringCompare (two, controlTwo));
	EtnLength length;
	rpcTestTwoArgsOneReturnReplyCall (h, cid, one, &length);
}

void rpcTestOneArgTwoReturns (EtnRpcHost *h, uint64 cid, String *one)
{
	xprint ("Executing RPC: $[string]\n", one);
	String *control = stringAllocateInitialize ("One");
	ASSERT (! stringCompare (one, control));
	String *two = stringAllocateInitialize ("Two");
	EtnLength length;
	rpcTestOneArgTwoReturnsReplyCall (h, cid, one, two, &length);
}

void rpcTestArrayArg (EtnRpcHost *h, uint64 cid, ByteArray arg)
{
	printf ("Executing RPC: %s\n", arg);
	ASSERT (! strcmp ((char *) arg, "foo"));
	EtnLength length;
	rpcTestArrayArgReplyCall (h, cid, arg, &length);
}

void rpcTestTupleArg (EtnRpcHost *h, uint64 cid, uint32 connectionId, String *value)
{
	xprint ("Executing RPC: $[string]\n", value);
	// First test passes "Value", second passes zero-length slice.
	String *controlOne = stringEmpty ();
	String *controlTwo = stringAllocateInitialize ("Value");
	ASSERT (! stringCompare (controlOne, value) || ! stringCompare (controlTwo, value));
	EtnLength length;
	rpcTestTupleArgReplyCall (h, cid, 0xdeadbeef, 0xfeedface, &length);
}

void rpcBenchmarkSendBool (EtnRpcHost *h, uint64 cid, bool arg)
{
	EtnLength length;
	rpcBenchmarkSendBoolReplyCall (h, cid, arg, &length);
}

void rpcBenchmarkSendUint8 (EtnRpcHost *h, uint64 cid, uint8_t arg)
{
	EtnLength length;
	rpcBenchmarkSendUint8ReplyCall (h, cid, arg, &length);
}

void rpcBenchmarkSendUint16 (EtnRpcHost *h, uint64 cid, uint16_t arg)
{
	EtnLength length;
	rpcBenchmarkSendUint16ReplyCall (h, cid, arg, &length);
}

void rpcBenchmarkSendUint32 (EtnRpcHost *h, uint64 cid, uint32_t arg)
{
	EtnLength length;
	rpcBenchmarkSendUint32ReplyCall (h, cid, arg, &length);
}

void rpcBenchmarkSendUint64 (EtnRpcHost *h, uint64 cid, uint64_t arg)
{
	EtnLength length;
	rpcBenchmarkSendUint64ReplyCall (h, cid, arg, &length);
}

void rpcBenchmarkSendFloat32 (EtnRpcHost *h, uint64 cid, float arg)
{
	EtnLength length;
	rpcBenchmarkSendFloat32ReplyCall (h, cid, arg, &length);
}

void rpcBenchmarkSendFloat64 (EtnRpcHost *h, uint64 cid, double arg)
{
	EtnLength length;
	rpcBenchmarkSendFloat64ReplyCall (h, cid, arg, &length);
}

void rpcBenchmarkSendString (EtnRpcHost *h, uint64 cid, String *arg)
{
	EtnLength length;
	rpcBenchmarkSendStringReplyCall (h, cid, arg, &length);
}

void rpcBenchmarkSendByteArray (EtnRpcHost *h, uint64 cid, uint8_t arg[])
{
	EtnLength length;
	rpcBenchmarkSendByteArrayReplyCall (h, cid, arg, &length);
}

void rpcBenchmarkSendUint32Array (EtnRpcHost *h, uint64 cid, uint32_t arg[])
{
	EtnLength length;
	rpcBenchmarkSendUint32ArrayReplyCall (h, cid, arg, &length);
}

void rpcBenchmarkSendCertificate (EtnRpcHost *h, uint64 cid, IdentityCertificate arg)
{
	EtnLength length;
	rpcBenchmarkSendCertificateReplyCall (h, cid, arg, &length);
}

void rpcBenchmarkSendTorture (EtnRpcHost *h, uint64 cid, Torture arg)
{
	EtnLength length;
	rpcBenchmarkSendTortureReplyCall (h, cid, arg, &length);
}

void rpcBenchmarkSendTreeNode (EtnRpcHost *h, uint64 cid, TreeNode arg)
{
	EtnLength length;
	rpcBenchmarkSendTreeNodeReplyCall (h, cid, arg, &length);
}

void rpcTestHelloReply (EtnRpcHost *h, uint64 cid)
{
	printf ("RPC Reply: Hello, world!\n");
}

void rpcTestSprintfReply (EtnRpcHost *h, uint64 cid, String *msg)
{
	xprint ("RPC Reply: $[string]\n", msg);
	String *control = stringAllocateInitialize ("Hello, parameterized world!");
	ASSERT (! stringCompare (msg, control));
}

void rpcTestTwoArgsOneReturnReply (EtnRpcHost *h, uint64 cid, String *one)
{
	xprint ("RPC Reply: $[string]\n", one);
	String *control = stringAllocateInitialize ("One");
	ASSERT (! stringCompare (one, control));
}

void rpcTestOneArgTwoReturnsReply (EtnRpcHost *h, uint64 cid, String *one, String *two)
{
	xprint ("RPC Reply: $[string] $[string]\n", one, two);
	String *controlOne = stringAllocateInitialize ("One");
	String *controlTwo = stringAllocateInitialize ("Two");
	ASSERT (! stringCompare (one, controlOne));
	ASSERT (! stringCompare (two, controlTwo));
}

void rpcTestArrayArgReply (EtnRpcHost *h, uint64 cid, ByteArray arg)
{
	printf ("RPC Reply: %s\n", arg);
	ASSERT (! strcmp ((char *) arg, "foo"));
}

void rpcTestTupleArgReply (EtnRpcHost *h, uint64 cid, uint64 status, uint64 eventId)
{
	printf ("RPC Reply: %llx %llx\n", status, eventId);
	ASSERT(status == 0xdeadbeef);
	ASSERT(eventId == 0xfeedface);
}

void rpcBenchmarkSendBoolReply (EtnRpcHost *h, uint64 cid)
{
}

void rpcBenchmarkSendUint8Reply (EtnRpcHost *h, uint64 cid, uint8_t arg)
{
}

void rpcBenchmarkSendUint16Reply (EtnRpcHost *h, uint64 cid)
{
}

void rpcBenchmarkSendUint32Reply (EtnRpcHost *h, uint64 cid)
{
}

void rpcBenchmarkSendUint64Reply (EtnRpcHost *h, uint64 cid)
{
}

void rpcBenchmarkSendFloat32Reply (EtnRpcHost *h, uint64 cid)
{
}

void rpcBenchmarkSendFloat64Reply (EtnRpcHost *h, uint64 cid)
{
}

void rpcBenchmarkSendStringReply (EtnRpcHost *h, uint64 cid, String *arg)
{
}

void rpcBenchmarkSendByteArrayReply (EtnRpcHost *h, uint64 cid)
{
}

void rpcBenchmarkSendUint32ArrayReply (EtnRpcHost *h, uint64 cid)
{
}

void rpcBenchmarkSendCertificateReply (EtnRpcHost *h, uint64 cid)
{
}

void rpcBenchmarkSendTortureReply (EtnRpcHost *h, uint64 cid)
{
}

void rpcBenchmarkSendTreeNodeReply (EtnRpcHost *h, uint64 cid)
{
}
