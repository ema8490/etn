//______________________________________________________________________________
// Packet oriented routines for describing, storing and retrieving from packet
//
//  The code is optimized for x86 architecture.  Hence, x86 data is written
//  natively using word or doubleword values, or simple memcpy.
//  This code does not yet support architectures which have different
//  data layout.
//______________________________________________________________________________

#ifndef __PACKET_H__
#define __PACKET_H__

#include <ethos/generic/assert.h>
#include <ethos/generic/list.h>
#include <ethos/dual/xalloc.h>

struct Packet_S {
    uint8_t          *_start; // beginning of packet
    ulong  	      _curr;  // current location in packet for reading
    ulong             _max;   // maximum number of bytes
    ListHead          _packetList;
    struct Tunnel_S  *tunnel;
    int               firstPacket;  // first packet of tunnel?
};

typedef struct Packet_S Packet;

// Request a new packet of the desired size
Packet* packetAllocate(uint32 size);
Packet* packetCopy(Packet *packet);
void    packetFree(Packet *);
void    packetSetAcknowledge(Packet *packet,  uint32 acknowledge);
void    packetSetLastPacket(Packet *packet);
  
// Packet Functions
void   resetCurrent(Packet *p);
uint32 remainingWritableBytes(Packet *p);
uint32 remainingReadableBytes(Packet *p);
uint32 filledBytes(Packet *p);
void   packetRemoveFromList(Packet *packet);
void   packetPrint(char *name, Packet *packet);
void   packetAppend(Packet *p, uint8_t *mem, ulong size);
void   packetInit(void);
void   packetSequenceAcknowledge(Packet *p, uint sequence, uint acknowledge);

static inline
uint32
getUint32 (Packet *p) 
{ 
    ASSERT(remainingReadableBytes(p) >= sizeof(uint32));

    uint32 val = *(uint32 *) (p->_start + p->_curr);
    p->_curr += sizeof(uint32);

    return val;
}

static inline
void
putUint32 (uint32 val, Packet *p)
{
    ASSERT(remainingWritableBytes(p) >= sizeof(uint32));

    char *where = xvectorIncrease(p->_start, sizeof (uint32));
    ASSERT(where);
    *(uint32 *) where = val;
}

// copy size bytes from one packet to another
// and advance current pointer on both.
static inline
void
putBytes(Packet *fp, uint32 size, Packet *tp)
{
    ASSERT(fp);
    ASSERT(tp);
    ASSERT(remainingWritableBytes(tp) >= size);

    packetAppend (tp, fp->_start + fp->_curr, size);
    fp->_curr += size;
}

#endif














