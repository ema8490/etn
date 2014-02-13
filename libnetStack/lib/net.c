//______________________________________________________________________________
// Network-related functions useful in kernel- and userspace
//______________________________________________________________________________

#include <rpcKernel.h>
#include <ethos/dual/ctype.h>
#include <ethos/generic/fmt.h>
#include <ethos/generic/net.h>

static int
_skip_atoi(char *s)
{
    int i=0;

    while (isdigit(*s))
	{
	    i = i*10 + *(s++) - '0';
	}
    return i;
}

//______________________________________________________________________________
/// return a char containing a hex digit 
//______________________________________________________________________________
char
toHex(int x)
{
	x &= 15;
	return  (x<= 9) ? (x + '0') : (x -10 +'a');
}

//______________________________________________________________________________
/// produce a printable string of an Ethernet MAC address 
//______________________________________________________________________________
bool
ethernetMacToString(EthernetMacString str, EthernetMac src)
{
    int i;
    int j=0;

    // Printable Source MAC.
    for (i = 0; i < EthernetMacSize; ++i)
	{
      
	    str[j++] = toHex(src[i]/16);
	    str[j++] = toHex(src[i]%16);
	    if (i<5)
		str[j++] = ':';
	}
    str[j] = '\0';

    return str;
}

//______________________________________________________________________________
/// convert a Ethernet MAC address string to an Ethernet MAC address 
//______________________________________________________________________________
bool
ethernetMacFromString(EthernetMac mac, const EthernetMacString str)
{
    sscanf((char *) (char *) str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
	   &mac[0],
	   &mac[1],
	   &mac[2],
	   &mac[3],
	   &mac[4],
	   &mac[5]);

    return true;
}

//______________________________________________________________________________
/// Convert src to string
//______________________________________________________________________________
bool
ipAddressToString(IpAddressString str,
		  IpAddress src
		  )
{
    unsigned char octets[4];

    octets[0] = src & 0xFF;
    octets[1] = (src >>  8) & 0xFF;
    octets[2] = (src >> 16) & 0xFF;
    octets[3] = (src >> 24) & 0xFF;

    snprint((char *) str, 16, "%u.%u.%u.%u",  octets[3], octets[2], octets[1], octets[0]);

    return true;
}

//______________________________________________________________________________
//  Convert an IP address in dotted string form to a 32-bit integer
//______________________________________________________________________________
IpAddress
ipAddressFromString(IpAddressString str)
{
    const char *c = (char *) str;
    int octets[4];
    char tmp[4];
    int j = 0;
    int k = 0;

    while(*c)
        {
            if (*c == '.')
                {

                    if (k > 3)
                        return 0 ;
                    tmp[k] = 0 ;
                    octets[j++] = _skip_atoi(tmp);
                    if (octets[j - 1] > 255 || octets[j - 1] < 0 )
                        return 0 ;
                    k = 0 ;
                }
            else
                {
                    tmp[k] = *c ;
                    k++ ;
                }
            c++ ;
        }
    if (k > 3)
        {
            return 0;
        }

    tmp[k] = 0 ;
    octets[j] = _skip_atoi(tmp);

    return (octets[0] * 16777216) + (octets[1] * 65536) + (octets[2] * 256) + (octets[3]);
}
