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
// it is not RPC-specific.
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

// FIXME: Right now, libetn requires that both client and server have all RPC functions defined.
void
rpcShadowDaemonPing (EtnRpcHost *h, uint64_t eventId)
{
    mixinPrint("received shadowdaemon ping\n");

    rpcCall (rpcShadowDaemonPingReplyCall, h, rpcInterfaceShadowDaemonPacketEncoder->connection, eventId, StatusOk);
}

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

void
rpcTerminalPingReply (EtnRpcHost *h, uint64_t eventId, Status status)
{
    mixinPrint("received terminal ping reply, status %d\n", status);
}

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
                                                     (EtnEncoder *) rpcInterfaceShadowDaemonPacketEncoder,
                                                     (EtnDecoder *) rpcInterfaceBufferDecoder);

    rpcInterfaceTerminalHost     = etnRpcHostNew(EtnToValue(&RpcTerminalServer, 0),
                                                     (EtnEncoder *) rpcInterfaceTerminalPacketEncoder,
                                                     (EtnDecoder *) rpcInterfaceBufferDecoder);

    rpcInterfaceKernelHost       = etnRpcHostNew(EtnToValue(&RpcKernelServer, 0),
                                                     (EtnEncoder *) rpcInterfaceKernelPacketEncoder,
                                                     (EtnDecoder *) rpcInterfaceBufferDecoder);

    rpcInterfaceNullHost         = etnRpcHostNew(EtnToValue(&RpcShadowDaemonServer, 0),
                                                     (EtnEncoder *) rpcInterfaceNullEncoder,
                                                     (EtnDecoder *) rpcInterfaceBufferDecoder);

    debugXPrint(rpcDebug, "done\n");
}

void
rpcInit(void)
{
    packetInit();

    // Initialize timer cache.
    reliabilityWindowInit();

    // initialize tunnel
    tunnelInit();

    rpcInitInterfaces();
}
