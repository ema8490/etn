#ifndef __ETHOS_RPC_H__
#define __ETHOS_RPC_H__

#include <ethos/generic/packetEncoder.h>
#include <ethos/generic/connection.h>
#include <ethos/generic/debug.h>
#include <ethos/dual/ethosTypes.h>
#include <ethos/dual/ref.h>


// Using extern keyword, C variables are declared but not defined. So, they must be defined in another header or c file before using them.
// Moreover, the extern extends the visibility to the whole program.
// By externing a variable we can use the variables anywhere in the program provided we know the declaration of them and the variable is defined somewhere.
// These global variables are used as hosts, encoders and decoders of Ethos primary components
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


// This function calls some other functions that initialize rpc components.
// The definition is inside rpc.c
void rpcInit(void);
