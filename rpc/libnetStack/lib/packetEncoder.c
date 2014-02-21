//______________________________________________________________________________
// Encode ETN directly into a packet, fragmenting if necessary. Implements the
// methods of an EtnEncoder.
//
// December 2011: W. Michael Petullo
//______________________________________________________________________________

#include <rpcKernel.h>
#include <ethos/generic/debug.h>
#include <ethos/dual/core.h>
#include <ethos/generic/connection.h>
#include <ethos/generic/packetEncoder.h>
#include <ethos/generic/netInterface.h>


// Definition of global variables declared in rpc.h
PacketEncoder    *rpcInterfaceShadowDaemonPacketEncoder;
PacketEncoder    *rpcInterfaceTerminalPacketEncoder;
PacketEncoder    *rpcInterfaceKernelPacketEncoder;
PacketEncoder    *rpcInterfaceNullEncoder;	/********* in rpc.h this pointer type is EtnNullEncoder ????? *********/
EtnBufferDecoder *rpcInterfaceBufferDecoder;
EtnRpcHost       *rpcInterfaceShadowDaemonHost;
EtnRpcHost       *rpcInterfaceTerminalHost;
EtnRpcHost       *rpcInterfaceKernelHost;
EtnRpcHost       *rpcInterfaceNullHost; // For calculating encoding lengths.

//______________________________________________________________________________
// Flush packet encoder, sending the packet as the final fragment.
//______________________________________________________________________________
static
void
_packetEncoderFlush (EtnEncoder *_e)
{
	PacketEncoder *e = (PacketEncoder *) _e; //VERY BAD CAST!! MEMORY ALLOCATION DEPENDENT!!

	ASSERT (e);
	ASSERT (e->packet);
	ASSERT (e->connection);

	debugXPrint (tunnelDebug,
	         "flushing ETN packet (payload length $[uint])\n",
		  filledBytes (e->packet));

	packetSetLastPacket (e->packet);

	tunnelOut (e->packet);
	e->packetSent = true;
}

//______________________________________________________________________________
// Create a packet of the maximum size allowed by the corresponding tunnel.
// Possibly declare as first fragment.
// Will increment sequence counter; if a packet is created here it must be sent.
//______________________________________________________________________________
static
void
_packetEncoderCreateMax (PacketEncoder *e, bool firstPacket)
{
	ASSERT (e);
	ASSERT (e->connection);

	int operation = (SendOnConnection << connectionOperationShift)
	              | (firstPacket ? connectionFirstPacket : 0);

	e->packet = tunnelPacketCreateMax(e->connection->tunnel);

	ASSERT (e->packet);

	// Operation and total size.
	putUint32(operation | e->totalLength, e->packet);
	putUint32(e->connection->connectionId, e->packet);
}

//______________________________________________________________________________
// Write data to a packet; send fragments as maximum size reached.
// The final (non-full) fragment is not sent because there may be remaining
// data to write to it. This fragment is sent when the flush function is called.
// Note that if the write does not fill the first fragment, then nothing will
// be sent until flush is called.
//______________________________________________________________________________
static
int
_packetEncoderWrite (EtnEncoder *_e, uint8_t *data, uint32_t length)
{
	ASSERT (_e);

	uint32_t origLength = length;

	PacketEncoder *e = (PacketEncoder *) _e; //VERY BAD CAST!! MEMORY ALLOCATION DEPENDENT!!

	if (NULL == e->packet) { // See commentary in packetEncoderReset.
		_packetEncoderCreateMax (e, true);
	}

	ASSERT(e->packet);

	uint32_t prevSize = filledBytes (e->packet);

	// Break packet into fragments of maximumPayloadSize; sending each.
	while (prevSize + length > e->connection->tunnel->maximumPayloadSize) {
		uint32_t n = e->connection->tunnel->maximumPayloadSize - prevSize;

		debugXPrint (tunnelDebug,
		         "fragmenting ETN packet (fragment size $[uint])\n",
		          n + prevSize);

		packetAppend (e->packet, data, n);
		data   += n;
		length -= n;

		ASSERT (filledBytes (e->packet) == n + prevSize);

		tunnelOut (e->packet);
		e->packetSent = true;

		packetEncoderReset (e, e->connection, e->totalLength);
		_packetEncoderCreateMax (e, false);

		prevSize = filledBytes (e->packet);
		ASSERT (prevSize == sizeof (uint32_t)   // Sequence number.
		                  + sizeof (uint32_t)   // Ack.
				  + sizeof (uint32_t)   // Operation.
				  + sizeof (uint32_t)); // Connection ID.
	}

	// Send remaining data.
	debugXPrint (tunnelDebug,
	         "encoding data of length $[uint] to ETN packet/final fragment (total=$[uint])\n",
		  length,
		  origLength);

	packetAppend (e->packet, data, length);

	ASSERT (filledBytes (e->packet) == length + prevSize);

	return origLength;
}

//______________________________________________________________________________
// Reset the packetEncoder.
//______________________________________________________________________________
void
packetEncoderReset (PacketEncoder *e, Connection *c, uint32_t totalLength)
{
	ASSERT (e);

	debugXPrint (tunnelDebug, "reseting ETN packet encoder\n");

	etnEncoderReset ((EtnEncoder *) e, _packetEncoderWrite, _packetEncoderFlush); //VERY BAD CAST!! MEMORY ALLOCATION DEPENDENT!!

	// NOTE: packets are freed in tunnelIsNewPacket() on remote acknowlegement.
	//       Free here if never sent.
	if (NULL != e->packet && false == e->packetSent) {
		packetFree (e->packet);
	}

	e->packet       = NULL; // Don't create packet here. Creating packet increments
	                        // seq. number and thus the kernel is committed to sending
				// the packet. Since packetEncoderReset is called by 
				// connectionProcessPacket, but not every received packet
				// warrants a reply, incrementing the seq. number prematurely
				// can cause out-of-order packets. Instead, create the
				// packet in write, where we know it will be sent eventually.
	e->totalLength  = totalLength;
	e->packetSent   = false;
	e->connection   = c;
	
	debugXPrint (tunnelDebug, "reset ETN packet encoder\n");
}

//______________________________________________________________________________
// Create a new, blank packetEncoder. Must be reset before use.
//______________________________________________________________________________
PacketEncoder *packetEncoderNew()
{
	PacketEncoder *e = malloc (sizeof (PacketEncoder));
	ASSERT(e);

	e->packet       = NULL;
	e->packetSent   = false;
	e->connection   = NULL;

	return e;
}
