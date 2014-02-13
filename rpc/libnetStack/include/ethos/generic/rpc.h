#ifndef __ETHOS_RPC_H__
#define __ETHOS_RPC_H__

#include <ethos/generic/packetEncoder.h>
#include <ethos/generic/connection.h>
#include <ethos/generic/debug.h>
#include <ethos/dual/ethosTypes.h>
#include <ethos/dual/ref.h>

extern PacketEncoder    *rpcInterfaceShadowDaemonPacketEncoder;
extern PacketEncoder    *rpcInterfaceTerminalPacketEncoder;
extern PacketEncoder    *rpcInterfaceKernelPacketEncoder;
extern EtnNullEncoder   *rpcInterfaceNullEncoder;
extern EtnBufferDecoder *rpcInterfaceBufferDecoder;
extern EtnRpcHost       *rpcInterfaceShadowDaemonHost;
extern EtnRpcHost       *rpcInterfaceTerminalHost;
extern EtnRpcHost       *rpcInterfaceKernelHost;
extern EtnRpcHost       *rpcInterfaceNullHost;

// A convenience macro: 
//       1. Calculate length of encoded packet with a dummy run.
//       2. Reset the packet encoder with this length.
//       3. Perform the actual RPC call.
// NOTE: ## is a GNU extension;
//       see http://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html.
#define rpcCall(fn, host, connection, eventId, ...) \
	{ \
	    EtnLength length; \
	    Status _status = fn (rpcInterfaceNullHost, eventId, ##__VA_ARGS__, &length); \
	    ASSERT (StatusOk == _status); \
	    PacketEncoder *encoder = (PacketEncoder *) etnRpcHostGetEncoder (host); \
	    packetEncoderReset (encoder, connection, length); \
	    debugXPrint(rpcDebug, #fn "\n"); \
	    _status = fn (host, eventId, ##__VA_ARGS__, &length); \
	    ASSERT (StatusOk == _status); \
	}

#endif

void rpcInit(void);
