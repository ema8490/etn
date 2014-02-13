//______________________________________________________________________________
// Retransmision timeouts calculation for network connections.
//
// May-2010: Ameet S Kotian
// Dec-2010: Jon A. Solworth
//______________________________________________________________________________

#include <rpcKernel.h>
#include <ethos/generic/assert.h>
#include <ethos/generic/debug.h>
#include <ethos/generic/fmt.h>
#include <ethos/generic/rtt.h>

//______________________________________________________________________________
/// computes number of milliseconds to wait for an acknowledgement
//______________________________________________________________________________
static
RttTime
_rttRtoCalculation(Rtt *rtt)
{
    ASSERT(rtt);
    // srtt + 4*rttVar
    return rtt->srtt + 4*rtt->rttVar;
}

//______________________________________________________________________________
/// make sure RTO is within range
//______________________________________________________________________________
static
RttTime
_rttMinMax(RttTime rto)
{
    if (rto < RTT_MIN)
	{
	    rto = RTT_MIN;
	}
    else if (rto > RTT_MAX)
	{
	    rto = RTT_MAX;
	}

    return rto;
}

//______________________________________________________________________________
// Initializes the rtt state and returns a handle
//______________________________________________________________________________
void 
rttInit(Rtt *rtt)
{
    ASSERT(rtt);

    rtt->rtt    = 0;
    rtt->srtt   = 0;
    rtt->rttVar = 25*RTT_MILLISECOND; // (originally 750ms, lowered to 25ms)
    // first RT at (srtt + (4 * rttvar)) = 100 milliseconds 
    rtt->rto    = _rttMinMax(_rttRtoCalculation(rtt));
}

RttTime
rttTimeFromTime(Time time)
{
    RttTime t = time.nanoseconds/1000000 + ((RttTime) time.seconds)*1000;
    return t;
}


RttTime
rttCurrentTime(void)
{
    static RttTime lastTime = 0;
    uint32_t seconds, nanoseconds;
    mixinGetTimeOfDay(&seconds, &nanoseconds);
    RttTime t = rttTimeFromTime((Time) { seconds, nanoseconds });
    if (t < lastTime)
	{
	    xprint("rttCurrentTime: time went backwards lastTime = $[xlonglong]  t = $[xlongLong]\n",
	            lastTime, t);
	    t = lastTime;
	}
    return t;
}

//______________________________________________________________________________
// This provides the retranmit waiting interval, and is called every time
// a packet is sent out.
//______________________________________________________________________________
RttTime
rttRto(Rtt *rtt)
{
    ASSERT(rtt);

    //debugXPrint("RTT after RTT Start, rto = $[int]\n", (uint32)(ptr->rto + 0.5));
    return rtt->rto;
}

//______________________________________________________________________________
// This is called when an acknowledgement is received for a once sent packet.
//______________________________________________________________________________
void 
rttUpdateRto(Rtt *rtt, RttTime ns)
{
    ASSERT(rtt);

    rtt->rtt = ns;

    // Update our estimators of RTT and mean deviation of RTT.
    //  See Jacobson's SIGCOMM' 88 paper, Appendix A, for the details.
    // Integer use in the kernel
    // delta = measureRTT - srtr
    // srtt = srtt + g * delta
    // rtvar = rtvar + h(|delta| -rtvar)
    // RTO = srtt + 4 * rttvar
    // where h = 1/4;  g = 1/8

    RttTime delta = rtt->rtt - rtt->srtt;
    rtt->srtt += delta/8;
    if (delta < 0)
	{
	    delta = -delta;
	}

    rtt->rttVar += (delta - rtt->rttVar) / 4 ; 	   // h = 1/4
    rtt->rto = _rttMinMax(_rttRtoCalculation(rtt));
    //debugXPrint("RTT after RTT stop, rto = $[int]\n", (uint32)(ptr->rto + 0.5));
}

//______________________________________________________________________________
// Needs to be called for resent packets. Causes exponential backoff.
//______________________________________________________________________________
void
rttTimeout(Rtt *rtt)
{
    ASSERT(rtt);

    rtt->rto = 2*rtt->rto;	    // double RTO 
    rtt->rto = _rttMinMax(rtt->rto); // make sure in range
    //debugXPrint("RTT exponential backoff, rto = $[int]\n", (uint32)(ptr->rto + 0.5));

    //  if at this point maximum number of retransmissions is reached, the connection should be torn down.
    //  NOTE: The conenction should be torn down now!
    //  return (-1);

    //debugXPrint("Now rto is is $[float] \n", ptr->rto);
}
