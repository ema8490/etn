//______________________________________________________________________________
/// Ethos-specific RPC components
//
// Jul-2009 Jon A. Solworth
//______________________________________________________________________________

#include <rpcKernel.h>
#include <ethos/dual/ref.h>
#include <ethos/dual/status.h>
#include <ethos/generic/connection.h>
#include <ethos/generic/rpc.h>
#include <ethos/generic/packetEncoder.h>
#include <ethos/generic/debug.h>

// This is here because it is used from both Ethos and Dom0 even though
// it is not RPC-specific. This is simply an array of debug flags
DebugFlagDesc debugFlagDesc[] = {
    { tunnelDebug, "debug.tunnel" },   // constant, kernel arg str
    { syscallDebug, "debug.syscall" },
    { eventDebug, "debug.event" },
    { fdDebug, "debug.fd" },
    { fileDebug, "debug.file" },
    { latchDebug, "debug.latch" },
    { processDebug, "debug.process" },
    { envelopeDebug, "debug.envelope" },
    { authenticateDebug, "debug.authenticate" },
    { memDebug, "debug.mem" },
    { typesDebug, "debug.types" },
    { rpcDebug, "debug.rpc" },
    { ipcDebug, "debug.ipc" },
    { ~0, "debug.all" },
    { 0, 0x00 }
};


/*********************************************/
/******* BEGIN DEBUG PURPOSE FUNCTIONS *******/
/*********************************************/


// These functions are used to send a ping between both Shadowdaemon and Terminal

// FIXME: Right now, libetn requires that both client and server have all RPC functions defined.
void
rpcShadowDaemonPing (EtnRpcHost *h, uint64_t eventId)
{
    mixinPrint("received shadowdaemon ping\n");

    rpcCall (rpcShadowDaemonPingReplyCall, h, rpcInterfaceShadowDaemonPacketEncoder->connection, eventId, StatusOk);
}

// Parameters h and eventId are useless in this function since they are not used
void
rpcShadowDaemonPingReply (EtnRpcHost *h, uint64_t eventId, Status status)
{
    mixinPrint("received shadowdaemon ping reply, status %d\n", status);
}

void
rpcTerminalPing (EtnRpcHost *h, uint64_t eventId)
{
    mixinPrint("received terminal ping\n");

    rpcCall (rpcTerminalPingReplyCall, h, rpcInterfaceTerminalPacketEncoder->connection, eventId, StatusOk);
}

// Parameters h and eventId are useless in this function since they are not used
void
rpcTerminalPingReply (EtnRpcHost *h, uint64_t eventId, Status status)
{
    mixinPrint("received terminal ping reply, status %d\n", status);
}


/*******************************************/
/******* END DEBUG PURPOSE FUNCTIONS *******/
/*******************************************/


// This function initialize the interfaces. All the variables that are initialized are global variables declared in rpc.h and defined in packetEncoder.c
// For each Ethos primary component a packet encoder is defined. Then, a null encoder and a buffer decoder are also defined.
// These five interfaces, in addiction to the servers of each primary component, are used to define the interface host of each
// primary component
void
rpcInitInterfaces(void)
{
    debugXPrint(rpcDebug, "Initializing RPC ...");

    // FIXME: How to get rid of these global variables?
    rpcInterfaceShadowDaemonPacketEncoder = packetEncoderNew();
    rpcInterfaceTerminalPacketEncoder     = packetEncoderNew();
    rpcInterfaceKernelPacketEncoder       = packetEncoderNew();
    rpcInterfaceNullEncoder               = etnNullEncoderNew();
    rpcInterfaceBufferDecoder             = etnBufferDecoderNew(NULL, 0);

    rpcInterfaceShadowDaemonHost = etnRpcHostNew(EtnToValue(&RpcShadowDaemonServer, 0),
                                                     (EtnEncoder *) rpcInterfaceShadowDaemonPacketEncoder, //VERY BAD CAST!! MEMORY ALLOCATION DEPENDENT!!
                                                     (EtnDecoder *) rpcInterfaceBufferDecoder); //VERY BAD CAST!! MEMORY ALLOCATION DEPENDENT!!

    rpcInterfaceTerminalHost     = etnRpcHostNew(EtnToValue(&RpcTerminalServer, 0),
                                                     (EtnEncoder *) rpcInterfaceTerminalPacketEncoder, //VERY BAD CAST!! MEMORY ALLOCATION DEPENDENT!!
                                                     (EtnDecoder *) rpcInterfaceBufferDecoder); //VERY BAD CAST!! MEMORY ALLOCATION DEPENDENT!!

    rpcInterfaceKernelHost       = etnRpcHostNew(EtnToValue(&RpcKernelServer, 0),
                                                     (EtnEncoder *) rpcInterfaceKernelPacketEncoder, //VERY BAD CAST!! MEMORY ALLOCATION DEPENDENT!!
                                                     (EtnDecoder *) rpcInterfaceBufferDecoder); //VERY BAD CAST!! MEMORY ALLOCATION DEPENDENT!!

    rpcInterfaceNullHost         = etnRpcHostNew(EtnToValue(&RpcShadowDaemonServer, 0),
                                                     (EtnEncoder *) rpcInterfaceNullEncoder, //VERY BAD CAST!! MEMORY ALLOCATION DEPENDENT!!
                                                     (EtnDecoder *) rpcInterfaceBufferDecoder); //VERY BAD CAST!! MEMORY ALLOCATION DEPENDENT!!

    debugXPrint(rpcDebug, "done\n");
}

// This function calls some functions that initialize rpc components.
void
rpcInit(void)
{
    // Initialize the packet
    packetInit();

    // Initialize timer cache
    reliabilityWindowInit();

    // Initialize tunnel
    tunnelInit();

    rpcInitInterfaces();
}
