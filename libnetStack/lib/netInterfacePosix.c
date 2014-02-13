//______________________________________________________________________________
/// Ethos-specific routines.
///
/// Jul 2012: W. Michael Petullo
//______________________________________________________________________________

#include <rpcKernel.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <ethos/generic/debug.h>
#include <ethos/dual/status.h>
#include <ethos/generic/net.h>
#include <ethos/generic/fmt.h>
#include <ethos/generic/packet.h>
#include <ethos/generic/netInterface.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Not defined by POSIX.
#define memzero(dest,size) memset(dest, 0x00, size)

// FIXME:
bool isTerminal = false;  // only used to check if set up ARP table.

bool haveSetEthosArpEntries;

void
netInterfaceSetup (bool doSetEthosArpEntries)
{
    haveSetEthosArpEntries = ! doSetEthosArpEntries;
}

NetInterface *
netInterfaceBind (uint headers,
		  char *networkIface,
		  const IpAddress localIp,
		  const uint16 localPort,
		  const CryptoKey *publicKey,
		  const CryptoKey *privateKey,
		  EtnInterfaceType *rpcInterface)
{
    static NetInterface *ni = NULL;

    // These headers are provided by POSIX socket.
    ASSERT (! (headers & HeaderEthernet));
    ASSERT (! (headers & HeaderIpUdp));

    EnvelopeLocal envelopeLocal;
    memzero((void *) &envelopeLocal, sizeof (EnvelopeLocal));
    memzero(envelopeLocal.mac, EthernetMacSize); // POSIX socket provides Ethernet frame.
    envelopeLocal.ipAddress = localIp;
    envelopeLocal.udpPort = localPort;
    if (publicKey)
	{
	    memcpy (envelopeLocal.ephemeralPublicKeyForIncoming, publicKey, CryptoBoxPublicKeySize);
	}
    else
	{
	    memzero(envelopeLocal.ephemeralPublicKeyForIncoming, CryptoBoxPublicKeySize);
	}
    if (privateKey) 
	{
	    memcpy (envelopeLocal.ephemeralPrivateKeyForIncoming, privateKey, CryptoBoxPrivateKeySize);
	}
    else 
	{
	    memzero(envelopeLocal.ephemeralPrivateKeyForIncoming, CryptoBoxPublicKeySize);
	}

    // Presently can only "Listen" once and before any "Dials".
    // This is because we only listen on one port; if a Dial already set this to
    // an ephemeral port, then we won't listen on el->udpPort.
    ASSERT (! ni || envelopeLocal.udpPort == 0);

    if (ni)
	{
	    // FIXME: Only one NetInterface for now.
	    return ni;
	}

    int socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd < 0)
	{
	    return NULL;
	}

    /* Allow fast reuse of ports; without this, Linux will not allow you to
     * reopen a port for some time after one shadowdaemon exits.
     */
    int reuse_true = 1;
    setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &reuse_true, sizeof(reuse_true));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(envelopeLocal.udpPort);
    addr.sin_addr.s_addr = htonl(envelopeLocal.ipAddress);

    /* NOTE: I was using connect to get an ephemeral port. In this case, one can
     * still use sendto to send to a different host. However, it seems the responses
     * were not processed by the Linux kernel properly. Watching with tcpdump showed:
     *
     * 22:35:17.117502 IP 127.0.0.1.57798 > 127.0.0.1.2001: UDP, length 80
     * 22:35:17.117527 IP 127.0.0.1 > 127.0.0.1: ICMP 127.0.0.1 udp port 2001 unreachable, length 116
     *
     * for incoming packets from a host other than the one "connected".
     *
     * This seems to confirm this:
     *
     *     http://www.minek.com/files/sock-faq/unix-socket-faq-5.html
     *     5.3 Does doing a connect() call affect the receive behaviourof the socket?
     *
     *     From Richard Stevens (rstevens@noao.edu):
     *
     *     Yes, in two ways. First, only datagrams from your "connected peer" 
     *     are returned. All others arriving at your port are not delivered
     *     to you.
     *
     * if (connect (socketFd, (struct sockaddr *) &addr, sizeof (addr)) < 0)
     * {
     *     close (socketFd);
     *     return NULL;
     * }
     *
     * Now I use bind with port 0 to get the ephemeral port.
     */
    if (bind (socketFd, (struct sockaddr *) &addr, sizeof (addr)) < 0) 
	{
	    close (socketFd);
	    return NULL;
	}

    // Set envelope port in case 0 was passed to this function (ephemeral).
    socklen_t addrlen = sizeof (addr);
    if (getsockname (socketFd, (struct sockaddr *) &addr, &addrlen) < 0) 
	{
	    close (socketFd);
	    return NULL;
	}
    envelopeLocal.udpPort   = ntohs (addr.sin_port);
	
    ni = netInterfaceInit(EtherNetInterfaceType,
			  headers,
			  &envelopeLocal,
			  rpcInterface);
	
    ni->fd = socketFd;

    ni->if_index = if_nametoindex(networkIface);
    ASSERT(ni->if_index);

    envelopeLocalPrint (&envelopeLocal);
    debugXPrint(tunnelDebug, "netInterfaceInit:  networkIface = '$[cstring]'   at index = '$[int]'\n",
		networkIface, ni->if_index);

    return ni;
}

Tunnel *
netInterfaceTunnel (NetInterface *interface,
                              const IpAddress remoteIp,
                              const uint16 remotePort,
                              const CryptoKey *publicKey,
                              EtnInterfaceType *rpcInterface)
{
    EnvelopeRemote envelopeRemote;
    memzero((void *) &envelopeRemote, sizeof (EnvelopeRemote));
    memzero(envelopeRemote.mac, EthernetMacSize); // POSIX socket provides Ethernet frame.
    envelopeRemote.ipAddress = remoteIp;
    envelopeRemote.udpPort = remotePort;
    if (publicKey)
	{
	    memcpy (envelopeRemote.ephemeralPublicKey, publicKey, CryptoBoxPublicKeySize);
	}
    else
	{
	    memzero(envelopeRemote.publicKey, CryptoBoxPublicKeySize);
	}

    envelopeRemotePrint (&envelopeRemote);
    
    return tunnelCreate (interface, &envelopeRemote, rpcInterface, true);
}

static
int
_getIfParam(int op, const char name[], struct ifreq *ifr)
{
    int fnval = 0;

    if (op == SIOCGIFADDR
	|| op == SIOCGIFHWADDR
	|| op == SIOCGIFNETMASK)
	{
	    int sock;

	    sock=socket(AF_INET, SOCK_STREAM, 0);
	    if (-1==sock)
		{
		    perror("socket failed");
		    exit(EXIT_FAILURE);;
		}

	    strncpy(ifr->ifr_name, name, sizeof(ifr->ifr_name)-1);
	    ifr->ifr_name[sizeof(ifr->ifr_name)-1]='\0';

	    if (-1==ioctl(sock, op, ifr))
		{
		    perror("ioctl failed");
		    exit(EXIT_FAILURE);;
		}

	    close(sock);
	    
	    fnval = 1;
	}

    return fnval;
}

IpAddress
netInterfaceGetLocalIpAddress(const char name[])
{
    struct ifreq ifr;
    IpAddressString ipS;

    if (! _getIfParam(SIOCGIFADDR, name, &ifr))
	{
	    fprintf(stderr, "getIfParam: operation not handled");
	    exit(EXIT_FAILURE);;
	}

    memzero(ipS, IpStringSize);
    strncpy ((char *) ipS, inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr), IpStringSize - 1);

    return ipAddressFromString(ipS);
}

int
netInterfaceGetRemoteMacAddress(unsigned int ifNum, uchar *result)
{
    char mac[20];
    EtnPathDecoder *d = NULL;

    memzero(mac, EthernetMacSize);

    if (ifNum == 0)
	{
	    d = etnPathDecoderNew("/etc/mac/meShadowDaemon");
	}
    else if (ifNum == 1)
	{
	    d = etnPathDecoderNew("/etc/mac/meNetwork");
	}
    else
	{
	    fprintf(stderr, "invalid interface number\n");
	    exit(EXIT_FAILURE);;
	}

    ASSERT (d);
    etnDecode ((EtnDecoder *) d, EtnToValue (&EthernetMacType, &result));
   
    return 1;
}

IpAddress
netInterfaceGetRemoteIpAddress(unsigned int ifNum)
{
    EtnPathDecoder *d = NULL;
    struct stat statBuf;
    IpAddress result;
    char *path1, *path2;

    if (ifNum == 0)
 	{
	    path1 = "/etc/ip/meShadowDaemon";
	    path2 = "/etc/ip/meShadowDaemon-auto-link-local";
	}
    else if (ifNum == 1)
 	{
	    path1 = "/etc/ip/meNetwork";
	    path2 = "/etc/ip/meNetwork-auto-link-local";
	}
    else
	{
	    fprintf(stderr, "invalid interface number\n");
	    exit(EXIT_FAILURE);;
	}

    if (stat (path1, &statBuf) == 0)
	{
	   d = etnPathDecoderNew(path1);
	}
    else if (stat (path2, &statBuf) == 0)
	{
	   d = etnPathDecoderNew(path2);
	}
    else
	{
	   fprintf(stderr, "could not read /etc/ip/meShadowDaemon or meShadowDaemon-auto-link-local.\n");
	   exit(EXIT_FAILURE);;
	}

    ASSERT (d);
    etnDecode ((EtnDecoder *) d, EtnToValue (&IpAddressType, &result));

    return result;
}

void
mixinGetRandomBytes(unsigned char *x, unsigned long long xlen)
{
    unsigned long long i = 0;
    unsigned long long numRead = 0;
    static int fd = -1;

    if (fd == -1)
	{
	    fd = open("/dev/urandom", O_RDONLY);
	    ASSERT(-1 != fd);
	}

    do
	{
	    numRead = read(fd, x + i, xlen - i);
	    i += numRead;
	} while (i < xlen);
}

void
netInterfaceBlocked(void)
{	    
}

void
netInterfaceAddArpTableEntry(IpAddress ipAddr, uchar8 *ethAddr)
{
    int sock;
    struct arpreq req;
    struct sockaddr_in *addr_inet;
    struct sockaddr    *addr_eth;
    IpAddressString ipAddrS;

    if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0) 
	{
	    perror("Error opening socket");
	    mixinExit (StatusFail);
	}

    memzero(&req, sizeof(req));

    addr_inet = (struct sockaddr_in *) &req.arp_pa;
    addr_inet->sin_family = AF_INET;
    addr_inet->sin_port = 0;
    addr_inet->sin_addr.s_addr = htonl (ipAddr);

    addr_eth = (struct sockaddr *) &req.arp_ha;
    addr_eth->sa_family = ARPHRD_ETHER;
    memcpy (addr_eth->sa_data, ethAddr, EthernetMacSize);

    req.arp_flags = ATF_PERM | ATF_COM;

    EthernetMacString ethAddrS;

    ipAddressToString(ipAddrS, ipAddr);
    ethernetMacToString(ethAddrS, ethAddr);
    printf("Adding ARP entry: %s %s\r\n", ipAddrS, ethAddrS);

    if (ioctl(sock, SIOCSARP, &req) < 0)
	{
	    perror("Error updating ARP table");
	    mixinExit (StatusFail);
	}
}

static
int
_netInterfaceIncomingOne(NetInterface *ni)
{
    socklen_t addrLen;
    struct sockaddr_in addr;
    static char buffer[ETH_FRAME_LEN];
    int socketFd = ni->fd;

    addrLen = sizeof(addr);
    int length = recvfrom(socketFd, buffer, ETH_FRAME_LEN, MSG_DONTWAIT, (struct sockaddr *) &addr, &addrLen);
    if (length < 0) 
	{  // no pending packet
	    return 0;
	}

    if (! haveSetEthosArpEntries && ! isTerminal)
        {
	    /* Set up static ARP table entries on Dom0 pointing to
	     * Ethos. This allows Dom0 to know Ethos' MAC address even though
	     * Ethos does not respond to ARP. We wait until we receive a
	     * packet from Ethos because this indicates that the Xen vifX.Y
	     * device is setup and we have a host-specific route pointing
	     * to Ethos. If we do this before, the ARP table entry will
	     * be associated with eth0 (em1, etc.) because the system will 
	     * incorrectly assume Ethos' IP address is out eth0 (em1, etc.).
	     *
	     * Note that the host-specific routes are set up by Xen's
	     * vif-route script (which is called by vif-ethos).
	     */

	    IpAddress ip;
	    EthernetMac mac;

	    ip = netInterfaceGetRemoteIpAddress(0);
	    netInterfaceGetRemoteMacAddress(0, mac);
	    netInterfaceAddArpTableEntry(ip, mac);

	    ip = netInterfaceGetRemoteIpAddress(1);
	    netInterfaceGetRemoteMacAddress(1, mac);
	    netInterfaceAddArpTableEntry(ip, mac);

	    haveSetEthosArpEntries = true;
	}

    ASSERT(length <= ETH_FRAME_LEN);
    EnvelopeBuffer eb = {buffer, length};
    EnvelopeLocal    envelopeLocal;
    EnvelopeRemote   envelopeRemote;
    memzero(&envelopeLocal,  sizeof(envelopeLocal));
    memzero(&envelopeRemote, sizeof(envelopeRemote));

    // Note: within the Ethos kernel, envelope information is extracted from an 
    // incoming packet.  But here we are not using Ethos' IP or UDP headers 
    // because we are layered on top of POSIX. So, we set up the IP/UDP 
    // components of the envelope and call envelopeOpenStartingAtCrypto()
    // instead of  envelopeOpen().
    envelopeRemote.udpPort = ntohs(addr.sin_port);
    envelopeRemote.ipAddress = ntohl(addr.sin_addr.s_addr);
    Packet *packet = envelopeOpenStartingAtCrypto(ni, eb, envelopeLocal, envelopeRemote);

    if (!packet)
	{
	    return 0 ;
	}
    netInterfaceReceivePacket(packet);

    return 1;
}


int
netInterfaceIncoming(NetInterface *ni)
{
    ASSERT(ni);
    ASSERT(EtherNetInterfaceType == ni->netInterfaceType);

    int anything = 0;
    while (_netInterfaceIncomingOne(ni))
	{
	    anything++;
	}

    return anything;
}

//______________________________________________________________________________
/// send out an ethernet (Linux) packet
//______________________________________________________________________________
bool
netInterfaceSend(Packet *packet)
{
    ASSERT(packet);

    bool fnval = false;
    Tunnel   *tunnel = packet->tunnel;
    ASSERT(tunnel);
    NetInterface *ni = tunnel->netInterface;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(tunnel->envelopeRemote.udpPort);
    addr.sin_addr.s_addr = htonl (tunnel->envelopeRemote.ipAddress);

    // send the packet
    char buffer[4096];
    
    EnvelopeBuffer eb = { buffer, 4096 };
    if (! envelopeCreate(tunnel, packet, &eb))
	{
	    goto done;
	}
    
    ASSERT(eb.size <= ETH_FRAME_LEN);
    ASSERT(eb.start == buffer);
    
    int socketFd = ni->fd;
    if (sendto(socketFd, eb.start, eb.size, 0, (struct sockaddr*) &addr, sizeof(addr)) < 0)
	{
	    perror("Send failed");
	    mixinExit (StatusFail);
	}

    fnval = true;

done:
    return fnval;
}

int
netInterfaceSendFull(NetInterface *ni)
{
    return 0;
}

bool
netInterfaceUpdateEphemeralKey (void)
{
    // FIXME: Do something here. Not required for shadowdaemon and terminal, though.
    return true;
}
