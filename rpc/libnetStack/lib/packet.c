//______________________________________________________________________________
//  Miscellaneous functions for managing packets.
//______________________________________________________________________________

#include <rpcKernel.h>
#include <ethos/generic/debug.h>
#include <ethos/dual/core.h>
#include <ethos/dual/xalloc.h>
#include <ethos/generic/buffer.h>
#include <ethos/generic/connection.h>
#include <ethos/generic/packet.h>
#include <ethos/generic/tunnel.h>
#include <ethos/generic/netInterface.h>

Xtype packetXtype;

//______________________________________________________________________________
//
//______________________________________________________________________________
void
packetInitialize(void *p)
{
    Packet *packet = (Packet *) p;
    packet->_start = NULL;
    packet->_curr = 0;
    packet->_max  = 0;
    packet->firstPacket = 0;

    INIT_LIST_HEAD(&packet->_packetList);
    ASSERT(list_empty(&packet->_packetList));

    packet->tunnel = NULL;
    packet->firstPacket = 0;
}

void
packetPrintXtype(void *p)
{
    Packet *packet = (Packet *) p;
    packetPrint("xtype: ", packet);
}

void
packetInit(void)
{
    packetXtype = xtype("Packet", sizeof(Packet), NULL, packetInitialize, packetPrintXtype);
}

//______________________________________________________________________________
// Packet and payload created
//______________________________________________________________________________
Packet*
packetAllocate(
	       uint32 size   ///< size in bytes
	       )
{
    ASSERT(size);
    ASSERT(packetXtype);
    Packet *packet = (Packet *) xalloc(packetXtype, 1);
    ASSERT(packet);

    packet->_start = (uint8_t *) bufferAllocateEmpty(size);
    ASSERT(packet->_start);

    packet->_max   = size;
    packet->_curr  = 0;
    return packet;
}

//______________________________________________________________________________
// initial setting of sequence and acknowledge
//______________________________________________________________________________
void
packetSequenceAcknowledge(Packet *p,
			  uint32  sequence,
			  uint32  acknowledge)
{
    ASSERT(p);
    ASSERT(p->_start);
    ASSERT(0 == xvectorCount(p->_start));
    
    char *where = xvectorIncrease(p->_start, 2*sizeof (uint32));
    ASSERT(where);
    ((uint32*) where)[0] = sequence;
    ((uint32*) where)[1] = acknowledge;
}

Packet*
packetCopy(Packet *packet)
{
    ASSERT(packet);
    Packet *p = packetAllocate(packet->_max);
    ASSERT(p);
    uint32 size = xvectorCount(packet->_start);
    packetAppend(p, packet->_start, size);
    p->_curr = packet->_curr;
    p->tunnel = packet->tunnel;
    return p;
}

//______________________________________________________________________________
// set the acknowledgement field of the packet (after packet already formed)
//______________________________________________________________________________
void
packetSetAcknowledge(Packet *p, uint32 acknowledge)
{
    ASSERT(xvectorCount(p->_start)  >= 2*sizeof(uint32));
    ((uint32*) p->_start)[1] = acknowledge;
}


//______________________________________________________________________________
//  Print packet contents
//______________________________________________________________________________
void
packetPrint(char *name, Packet *packet)
{
    ASSERT(name);
    ASSERT(packet);

    ulong size = xvectorCount(packet->_start);
    if (size < 2*sizeof(uint32))
	{ 
	    debugXPrint(tunnelDebug, "$[cstring] only has $[int] bytes\n", name, size);
	}
    uint32      sequence    = ((uint32*) packet->_start)[0];
    uint32      acknowledge = ((uint32*) packet->_start)[1];
    Tunnel          *tunnel = packet->tunnel;
    EnvelopeRemote  er      = tunnel->envelopeRemote;
    debugXPrint(tunnelDebug, "sequence = $[uint]   acknowledge = $[uint], remoteUdpPort = ($[uint])\n", sequence, acknowledge, er.udpPort);
}


void
packetFree(Packet *packet)
{
    ASSERT(packet);
    ASSERT(packet->_start);
    ASSERT(list_empty(&packet->_packetList));
    
    bufferFree(packet->_start); // free buffer
    packet->_curr = 0;
    packet->_start = 0;
    xfree(packet);         // free packet

}

void
packetRemoveFromList(Packet *packet)
{
    list_del_init(&packet->_packetList);
    ASSERT(list_empty(&packet->_packetList));
}

void
resetCurrent(Packet *p)
{
    ASSERT(p);
    p->_curr = 0;
}

uint32
filledBytes (Packet *p)
{
    ASSERT(p);
    return xvectorCount(p->_start);
}

uint32
remainingReadableBytes(Packet *p) 
{ 
    ASSERT(p);
    return xvectorCount(p->_start) - p->_curr;
}

uint32
remainingWritableBytes(Packet *p) 
{ 
    ASSERT(p);
    return p->_max - xvectorCount(p->_start);
}

void
packetAppend(Packet *p, uint8_t *mem, ulong size)
{
    ASSERT(p);
    ASSERT(p->_start);
    ASSERT(remainingWritableBytes(p) >= size);
    uchar *where = xvectorIncrease(p->_start, size);
    ASSERT(where);
    memcpy(where, mem, size);
}

//______________________________________________________________________________
// set the last packet bit of the packet (after packet already formed)
//______________________________________________________________________________
void
packetSetLastPacket(Packet *p)
{
    ASSERT(p);
    ASSERT(xvectorCount(p->_start) >= 3 * sizeof (uint32_t));

    ((uint32_t *) p->_start)[2] |= connectionLastPacket;
}
