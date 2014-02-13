//______________________________________________________________________________
/// reliability window for networking
//
//  Sep-2010: Ameet S. Kotian
//______________________________________________________________________________

#ifndef __RELIABILITY_WINDOW_H__
#define __RELIABILITY_WINDOW_H__

#include <ethos/dual/ethosTypes.h>
#include <ethos/generic/list.h>
#include <ethos/generic/rtt.h>

typedef struct ReliabilityWindow_S {
    Packet          *packet;                ///< packet

    RttTime          timestamp;             ///< Note the time when the packet was sent.
    int              transmitCount;         ///< Number of times packet is transmitted/retransmitted.

    RttTime          retransmitTime;        ///< time at which to retransmit if ack not received

    ListHead         retransmitList;        ///< list of elements to be retransmitted on time queue
} ReliabilityWindow;

void    reliabilityWindowInit(void) ;
void    reliabilityWindowAdd(ReliabilityWindow *rw, Rtt *rtt);
void    reliabilityWindowRemove(ReliabilityWindow *rw) ;
void    reliabilityWindowDo(void);


ReliabilityWindow *reliabilityWindowFromList(ListHead *l);
#endif
