//______________________________________________________________________________
/// Manages connection over limited packet-size transports
//
//  Aug-2009: Jon A. Solworth
//______________________________________________________________________________

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <ethos/generic/packet.h>
#include <ethos/generic/tunnel.h>

enum Connection_E {
    NewConnection=0,
    AcceptConnection=1,
    RejectConnection=2,
    CloseConnection=3,
    SendOnConnection=4,
    SendOnConnectionF=5,
    SendOnConnectionL=6,
    SendOnConnectionFL=7
};

static const uint32 connectionOperationShift = 29;
static const uint32 connectionFirstPacket    = 1<<29;
static const uint32 connectionLastPacket     = 1<<30;
static const uint32 connectionSizeMask       = (1<<29) - 1;


typedef struct Connection_S Connection;

enum ConnectionStatus_E {
    ConnectionUnused,
    ConnectionInitiated,
    ConnectionEstablished
};

typedef enum ConnectionStatus_E ConnectionStatus;
    
struct Connection_S {
    uint32           connectionId;     ///< outgoing connection number
    ConnectionStatus connectionStatus; ///< status of connection
    Connection      *toConnection;     ///< for either split or merge, next connection
    uint32           next;             ///< linked list of incoming descriptors
    Tunnel          *tunnel;           ///< the tunnel the connection runs over
    Packet          *inPacket;         ///< an incoming packet being assembled
    RdId             ipcRdId;          ///< the local RdId for this connection
};

// general purpose routines 
int mySideConnectionId(Tunnel *, uint32);

void connectionProcessPacket(Connection *connection, Packet *packet);

Connection *connectionAlloc(void);
void connectionFree(Connection *);
void sendOnConnection(Connection *connection, Packet *packet);
void acceptConnection(Connection *connection);
Connection *newConnection(Tunnel *tunnel, bool firstPacket);
void closeConnection(Connection *connection);
void rejectConnection(Tunnel *tunnel, uint32 connectionId);
#endif
