//______________________________________________________________________________
// Reliability window timer for network packets timeouts.
//
// Nov-2010: Jon A. Solworth
//______________________________________________________________________________

#include <rpcKernel.h>
#include <ethos/generic/debug.h>
#include <ethos/dual/ethosTypes.h>
#include <ethos/generic/tunnel.h>
#include <ethos/generic/reliabilityWindow.h>
#include <ethos/generic/netInterface.h>

// unexpired timers
// maximum time in the future that a packet timer can go off
// is less than RTT_BUCKETS. 
static ListHead timer[RTT_BUCKETS]; // timers for the next RTT_MAX seconds
static RttTime  lastTime = 0;       // last time the timer array was checked for expiration

ReliabilityWindow*
reliabilityWindowFromList(ListHead *l)
{
    ASSERT(l);

    return (ReliabilityWindow *) list_entry(l, ReliabilityWindow, retransmitList);
}

//______________________________________________________________________________
/// stage timers
// It might have been a while since we've looked at the reliability window,
// that is, lastTime might be less that the current time.
// So whenever we look at the reliability window we first bring lastTime
// up to current time by moving all those packets to an expired list.
// After that, we are using time slots from
//      lastTime%RTT_BUCKETS  ... (lastTime+RTT_MAX)%RTT_BUCKETS
//______________________________________________________________________________
void
reliabilityWindowDo(void)
{
    RttTime timeNow     = rttCurrentTime();
    ASSERT(timeNow >= lastTime);

    RttTime ticks = timeNow - lastTime;
    uint bucketLimit = ticks > RTT_BUCKETS ? RTT_BUCKETS : ticks;

    //debugXPrint(tunnelDebug, "lastTime = $[llint]    timeNow = $[llint]\n",
    //    lastTime, timeNow);
    
    for (; bucketLimit--; lastTime++)
	{   // seconds have changed, do all the remaining (expired) short term timers	
	    ListHead *q, *tmp;
	    list_for_each_safe(q, tmp, &timer[lastTime%RTT_BUCKETS])
		{
		    ReliabilityWindow *rw = reliabilityWindowFromList(q);
		    NetInterface      *ni = rw->packet->tunnel->netInterface;
		    //debugXPrint(tunnelDebug, "retransmit timer expired\n");

		    list_del_init(q);
		    list_add_tail(q, &ni->expiredRwSlots);
		}
	    ASSERT(list_empty(&timer[lastTime%RTT_BUCKETS]));
	}
    lastTime = timeNow;
}


//______________________________________________________________________________
// Initialize an array of timers.
//______________________________________________________________________________
void 
reliabilityWindowInit(void)
{
    debugXPrint(tunnelDebug, "Initializing timers\n");

    uint i;
    for (i=0; i< RTT_BUCKETS; i++)
	{
	    INIT_LIST_HEAD(&timer[i]);
	}

    lastTime = rttCurrentTime();
}



//______________________________________________________________________________
// Add a timer associated.
//______________________________________________________________________________
void
reliabilityWindowAdd(ReliabilityWindow *rw, Rtt *rtt) 
{
    ASSERT(rw);
    ASSERT(RTT_MAX <= RTT_BUCKETS);

    ASSERT(list_empty(&rw->retransmitList));

    rw->timestamp      = rttCurrentTime();
    rw->retransmitTime = rw->timestamp + rttRto(rtt);

    //debugXPrint(tunnelDebug, "timestamp = $[llint]   retransmit = $[llint]   rto = $[llint]\n",
    //rw->timestamp, rw->retransmitTime, rw->retransmitTime - rw->timestamp);

    reliabilityWindowDo(); // ensure list invariants hold

    if (rw->retransmitTime < lastTime)
	{
	    NetInterface      *ni = rw->packet->tunnel->netInterface;
	    
	    debugXPrint(tunnelDebug, "expired rw slot\n");
	    list_add_tail(&rw->retransmitList, &ni->expiredRwSlots);
	}
    else
	{
	    ASSERT((rw->retransmitTime - lastTime) < RTT_BUCKETS);
	    
	    list_add_tail(&rw->retransmitList, &timer[rw->retransmitTime%RTT_BUCKETS]);
	}
}

//______________________________________________________________________________
// Remove a timer 
//______________________________________________________________________________
void 
reliabilityWindowRemove(ReliabilityWindow *rw)
{
    ASSERT(rw);

    list_del_init(&rw->retransmitList);
    ASSERT(list_empty(&rw->retransmitList));
}
