//______________________________________________________________________________
/// Round trip time estimator
//
//  Aug-2010: Ameet S. Kotian
//  Dec-2010: Jon A. Solworth
//______________________________________________________________________________
#ifndef __RTT_H__
#define __RTT_H__

// From  UNIX Network Programming : Volume I

// this is in milliseconds
#define RTT_MILLISECOND 1           // one millisecond
#define RTT_SECOND      1000        // one second 
typedef int64 RttTime;              // in millseconds since the Epoch

typedef struct Rtt_S {
    RttTime    rtt;     // most recent measured RTT
    RttTime    srtt;    // smoothed RTT estimator
    RttTime    rttVar;  // smoothed mean deviation
    RttTime    rto;     // current RTO to use
} Rtt;


#define RTT_BUCKETS   (1<<16)                                   // number of buckets, must be > RTT_MAX
#define RTT_MIN       ((RttTime) 1*RTT_MILLISECOND)             // min retransmit timeout value (1 millisecond)
#define RTT_MAX       ((RttTime) (60*RTT_SECOND))               // max retransmit timeout value (60 seconds)
#define RTT_MAXNREXMT 3                                         // max #times to retransmit

void    rttInit(Rtt *ptr );
RttTime rttRto(Rtt *ptr);
void    rttUpdateRto(Rtt *ptr, RttTime ns);
void    rttTimeout(Rtt *ptr);
RttTime rttTimeFromTime(Time t);
RttTime rttCurrentTime(void);

#endif
