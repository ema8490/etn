//______________________________________________________________________________
/// Manages connection over limited packet-size transports
//
//  Aug-2009: Jon A. Solworth
//______________________________________________________________________________

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <ethos/generic/packet.h>
#include <ethos/generic/tunnel.h>

/* Operation Codes */
enum Connection_E {
    NewConnection=0, // request for a new connection
    AcceptConnection=1, // accept an incoming connection
    RejectConnection=2, // reject an incoming connection
    CloseConnection=3, // close a connection 
    SendOnConnection=4, // send on a connection a middle packet
    SendOnConnectionF=5, // send on a connection the first packet
    SendOnConnectionL=6, // send on a connection the last packet
    SendOnConnectionFL=7 // send on a connection a single packet (first and last packet)
};

/* Constant used for coding inside a packet an operation code */
static const uint32 connectionOperationShift = 29;
/* Constants used for coding inside a packet information about whether it is the first or last packet of a Connection */
static const uint32 connectionFirstPacket    = 1<<29;
static const uint32 connectionLastPacket     = 1<<30;
static const uint32 connectionSizeMask       = (1<<29) - 1;


typedef struct Connection_S Connection;

/* Status of a Connection */
enum ConnectionStatus_E {
    ConnectionUnused,
    ConnectionInitiated,
    ConnectionEstablished
};

typedef enum ConnectionStatus_E ConnectionStatus;

/* Represents a Connection */
struct Connection_S {
    uint32           connectionId;     ///< outgoing connection number, is tunnel specific (so, it is the same for both sides)
    ConnectionStatus connectionStatus; ///< status of connection
    Connection      *toConnection;     ///< for either split or merge, next connection
    uint32           next;             ///< linked list of incoming descriptors
    Tunnel          *tunnel;           ///< the tunnel the connection runs over
    Packet          *inPacket;         ///< an incoming packet being assembled
    RdId             ipcRdId;          ///< the local RdId for this connection
};

/* GENERAL PURPOSE ROUTINES */
/* Given a Tunnel and an outgoing connection number, checks whether they match (i.e. connectionId and tunnel oddSide are both even or odd) */
int mySideConnectionId(Tunnel *, uint32);
/* Once all the fragments (network packets) have been received, they are joined into a memory packets and handled by rpcInterfaceTerminalHost, rpcInterfaceShadowDaemonHost, rpcInterfaceKernelHost respectively if the connection tunnel is associated to RpcTerminalServer, RpcShadowDaemonServer, RpcKernelServer */
void connectionProcessPacket(Connection *connection, Packet *packet);

/* Creates a new connection allocating the needed memory for the Connection struct */
Connection *connectionAlloc(void);

/* Free the allocated memory for the given Connection */
void connectionFree(Connection *);

/* NOT USED */
void sendOnConnection(Connection *connection, Packet *packet);

/* Given an incoming Connection, accepts it sending a packet with AcceptConnection code as operation and the connectionId */
void acceptConnection(Connection *connection);

/* Creates a new connection associated with the passed tunnel. If there is also a first packet to be sent (firstPacket = true) then it is sent with the NewConnection operation code and the new connectionId (and packet->firstPacket flag is set to true) */
Connection *newConnection(Tunnel *tunnel, bool firstPacket);

/* Given a Connection, closes it sending a packet with CloseConnection code as operation and the connectionId */
void closeConnection(Connection *connection);

/* Given an incoming Connection, rejects it sending a packet with RejectConnection code as operation and the connectionId */
void rejectConnection(Tunnel *tunnel, uint32 connectionId);
#endif
