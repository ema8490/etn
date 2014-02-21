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

// Create a new, blank packetEncoder. Must be reset before use.
// The definition is inside packetEncoder.c
PacketEncoder *packetEncoderNew(void);

// Reset the packetEncoder.
// The definition is inside packetEncoder.c
void           packetEncoderReset(PacketEncoder *e, Connection *c, uint32_t totalLength);

// Write data to a packet.
// Inside packetEncoder.c there is the definition of _packetEncoderWrite (which is static) ?????
int            packetEncoderWrite(EtnEncoder *_e, uint8_t *data, uint32_t length);

// Flush packet encoder, sending the packet as the final fragment.
// Inside packetEncoder.c there is the definition of _packetEncoderFlush (which is static) ?????
void           packetEncoderFlush(EtnEncoder *e);

#endif
