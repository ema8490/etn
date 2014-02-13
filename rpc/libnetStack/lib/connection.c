//______________________________________________________________________________
/// Tunnels can support multiple connections
//
// Jul-2009: Jon A. Solworth
//______________________________________________________________________________

#include <rpcKernel.h>
#include <ethos/generic/debug.h>
#include <ethos/dual/status.h>
#include <ethos/dual/core.h>
#include <ethos/generic/rpc.h>
#include <ethos/generic/connection.h>
#include <ethos/generic/tunnel.h>
#include <ethos/generic/netInterface.h>
#include <ethos/generic/netCrypto.h>
#include <ethos/generic/packetEncoder.h>

Connection *
connectionAlloc(void)
{
    Connection *connection = (Connection *) malloc(sizeof(Connection));
    memzero((void *)connection, sizeof(Connection));

    return connection;
}

void
connectionFree(Connection *connection)
{
    ASSERT(connection);
    free(connection);
}

//______________________________________________________________________________
// connection IDs specify the side of the connection, check that it matches
//______________________________________________________________________________
int
mySideConnectionId(Tunnel *tunnel, uint32 connectionId)
{
    ASSERT(tunnel);

    int connectionParity = connectionId & 1;
    return tunnel->envelopeRemote.oddSide == connectionParity;
}

//_____________________________________________________________________________
/// Collected all the network packets of the connection into a memory packet,
/// now process it
//_____________________________________________________________________________
void
connectionProcessPacket(Connection *connection,
			Packet     *packet
			)
{
    ASSERT(connection);
    ASSERT(packet);

    debugXPrint(rpcDebug, "reset decoder buffer (length is $[int] bytes)\n", xvectorCount(packet->_start));
    etnBufferDecoderReset (rpcInterfaceBufferDecoder, (uint8_t *) packet->_start, xvectorCount(packet->_start));

    // FIXME: Everything uses rpcInterfaceShadowDaemonHost (not rpcInterfaceTerminalHost) this is
    //        because this function is not able to differentiate between packets coming in on the
    //        shadow daemon interface---in this case, everything comes in over the control connection.
    // FIXME: zero argument to packetEncoderReset should be okay because etnRpcHandle should only directly
    //        decode, not encode. We will reset again before a *ReplyCall with the correct
    //        length, but we need to reset here to get the connection.
    if (connection->tunnel->rpcInterface == &RpcTerminalServer)
	{
	    packetEncoderReset (rpcInterfaceTerminalPacketEncoder, connection, 0);
	    debugXPrint(rpcDebug, "call etnRpcHandle [rpcInterfaceTerminalHost]\n");
	    etnRpcHandle (rpcInterfaceTerminalHost);
	}
    else if (connection->tunnel->rpcInterface == &RpcShadowDaemonServer)
	{
	    packetEncoderReset (rpcInterfaceShadowDaemonPacketEncoder, connection, 0);
	    debugXPrint(rpcDebug, "call etnRpcHandle [rpcInterfaceShadowDaemonHost]\n");
	    etnRpcHandle (rpcInterfaceShadowDaemonHost);
	}
    else if (connection->tunnel->rpcInterface == &RpcKernelServer)
	{
	    packetEncoderReset (rpcInterfaceKernelPacketEncoder, connection, 0);
	    debugXPrint(rpcDebug, "call etnRpcHandle [rpcInterfaceKernelHost]\n");
	    etnRpcHandle (rpcInterfaceKernelHost);
	}
    else
	{
	    debugXPrint(rpcDebug, "Unknown ETN RPC associated with tunnel\n");
	    BUG ();
	}
}

//______________________________________________________________________________
/// accept a new connection request
//______________________________________________________________________________
void
acceptConnection(Connection *connection)
{
    ASSERT(connection);

    Tunnel    *tunnel = connection->tunnel;
    ASSERT(tunnel);

    Packet *packet = tunnelPacketCreate(tunnel, 0);

    uint32 operation = AcceptConnection << connectionOperationShift;
    putUint32(operation,    packet);
    putUint32(connection->connectionId, packet);

    // the other side allocated
    ASSERT(!mySideConnectionId(tunnel, connection->connectionId));

    tunnelOut(packet);
}


//______________________________________________________________________________
/// request a new connection within a tunnel.
/// first packet is true only if tunnel is initiated from this side and
/// this is the initial request on the tunnel.
//______________________________________________________________________________
void
closeConnection(Connection *connection)
{
    ASSERT(connection);
    Tunnel *tunnel = connection->tunnel;
    NetInterface *netInterface = tunnel->netInterface;
    ASSERT(netInterface);

    // allocates enough space for reliability and connection headers
    Packet *packet = tunnelPacketCreate(tunnel, 0);

    ASSERT(packet);

    // size of new connection is zero, since there is no
    // authentication data here
    uint32 operation    = CloseConnection << connectionOperationShift;
    uint32 connectionId = connection->connectionId;
    ASSERT(connectionId);

    // this side allocated
    ASSERT(mySideConnectionId(tunnel, connectionId));

    debugXPrint(tunnelDebug, "closeConnection: netInterfaceNumber = $[int]    connectionId = $[int]\n",
	     netInterface->interfaceNumber,
	     connectionId);

    putUint32(operation,    packet);
    putUint32(connectionId, packet);
    tunnelOut(packet);
}
//______________________________________________________________________________
/// request a new connection within a tunnel.
/// first packet is true only if tunnel is initiated from this side and
/// this is the initial request on the tunnel.
//______________________________________________________________________________
Connection *
newConnection(Tunnel *tunnel, bool firstPacket)
{
    ASSERT(tunnel);

    NetInterface *netInterface = tunnel->netInterface;
    ASSERT(netInterface);

    // size of new connection is zero, since there is no
    // authentication data here
    uint32 operation    = NewConnection << connectionOperationShift;
    uint32 connectionId = tunnelConnectionAllocate(tunnel);
    if (!connectionId)
	{
	    return NULL;
	}

    // this side allocated
    ASSERT(mySideConnectionId(tunnel, connectionId));

    debugXPrint(tunnelDebug, "newConnection: netInterfaceNumber = $[int]    connectionId = $[int]\n",
	     netInterface->interfaceNumber,
	     connectionId);

    // allocates enough space for reliability and connection headers.
    Packet *packet = firstPacket 
                   ? tunnelPacketCreateFirst(tunnel, 0) 
                   : tunnelPacketCreate(tunnel, 0);
    ASSERT(packet);

    putUint32(operation,    packet);
    putUint32(connectionId, packet);

    tunnelOut(packet);

    return tunnel->connectionArray[connectionId];
}

//______________________________________________________________________________
/// refuse a new connection request
//______________________________________________________________________________
void
rejectConnection(Tunnel *tunnel, uint32 connectionId)
{
    ASSERT(tunnel);
    ASSERT(tunnel->netInterface);

    // the other side allocated
    ASSERT(!mySideConnectionId(tunnel, connectionId));

    uint32 operation = RejectConnection << connectionOperationShift;
    Packet *packet = tunnelPacketCreate(tunnel, 8);

    putUint32(operation,    packet);
    putUint32(connectionId, packet);
    tunnelOut(packet);
}
