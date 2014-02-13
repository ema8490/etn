//______________________________________________________________________________
// Encode ETN directly into a packet, fragmenting if necessary. Implements the
// methods of an EtnEncoder.
//
// December 2011: W. Michael Petullo
//______________________________________________________________________________

#ifndef __PACKET_ENCODER_H__
#define __PACKET_ENCODER_H__

#include <ethos/generic/connection.h>
#include <ethos/generic/packet.h>

// Encoder subclass that encodes to packet and sends it on flush.
typedef struct PacketEncoder_s {
	EtnBufferEncoder encoder;
	Packet          *packet;
	uint32_t         totalLength;
	bool             packetSent;
	Connection      *connection;
} PacketEncoder;

PacketEncoder *packetEncoderNew(void);
void           packetEncoderReset(PacketEncoder *e, Connection *c, uint32_t totalLength);
int            packetEncoderWrite(EtnEncoder *_e, uint8_t *data, uint32_t length);
void           packetEncoderFlush(EtnEncoder *e);

#endif
