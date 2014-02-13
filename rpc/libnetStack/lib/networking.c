//______________________________________________________________________________
// Commmon networking functions.
//
// May-2010: Ameet S Kotian
//______________________________________________________________________________

#include <ethos/generic/assert.h>
#include <ethos/generic/debug.h>
#include <ethos/generic/networking.h>

#if defined(__BIG_ENDIAN__) 

uint16 htons(uint16 n) { return n; }
uint16 ntohs(uint16 n) { return n; }
uint32 htonl(uint32 n) { return n; }
uint32 ntohl(uint32 n) { return n; }

#elif defined(__LITTLE_ENDIAN__)

uint16
htons(uint16 n)
{
    return ((n & 0xFF) << 8) | ((n & 0xFF00) >> 8); 
}

uint16
ntohs(uint16 n)
{
    return ((n & 0xFF) << 8) | ((n & 0xFF00) >> 8); 
}

uint32
htonl(uint32 n)
{
    return ((n & 0xFF) << 24) |
	    ((n & 0xFF00) << 8) |
	    ((n & 0xFF0000) >> 8) |
	    ((n & 0xFF000000) >> 24);
}

uint32
ntohl(uint32 n)
{
    return ((n & 0xFF) << 24) |
	    ((n & 0xFF00) << 8) |
	    ((n & 0xFF0000) >> 8)  |
	    ((n & 0xFF000000) >> 24);
}
#endif

//______________________________________________________________________________
// Subroutine to calculate checksum. Used in both IP and UDP headers.
// From UNIX Network Programming - Richard Stevens, Volume 1.
//______________________________________________________________________________
uint16_t
internetChecksum(uint16 *addr, int len)
{
    ASSERT(addr);
 
    int                     nleft = len;
    uint32                  sum = 0;
    uint16                  *w = addr;
    uint16                  answer = 0;

    // Our algorithm is simple, using a 32 bit accumulator (sum), we add
    // sequential 16 bit words to it, and at the end, fold back all the
    // carry bits from the top 16 bits into the lower 16 bits.
    while (nleft > 1)  
	{
	    sum += *w++;
	    nleft -= 2;
	}

    // Mop up an odd byte, if necessary.
    if (nleft == 1) 
	{
	    *(unsigned char *) (&answer) = *(unsigned char *) w;
	    sum += answer;
	}

    // Add back carry outs from top 16 bits to low 16 bits.
    sum  = (sum >> 16) + (sum & 0xffff);    // Add hi 16 to low 16.
    sum += (sum >> 16);                     // Add carry.
    answer = ~sum;                          // Truncate to 16 bits.

    return answer;
}
