U ?= $(shell pwd)/..

include $(U)/mk/bootstrap.mk
include $(MAKEDIR)/sugarTop.mk

lib_DUALLIBRARIES=\
	libnetStack\

lib_LIBRARIES=\
	libnetStackPosix

libnetStack_OBJS=\
	crypto.o\
	ethernet.o\
	connection.o\
	packet.o\
	packetEncoder.o\
	envelope.o\
	networking.o\
	udp.o\
	rpc.o\
	ip.o\
	net.o\
	rtt.o\
	netCrypto.o\
	tunnel.o\
	reliabilityWindow.o\
	netInterface.o\

libnetStack_CPPFLAGS=\
	-I../include\
	-I../../libkernelTypes\
	-I../../libnet/include

libnetStack_LDFLAGS=\
	-L../libnet

libnetStackPosix_OBJS=\
	netInterfacePosix.o

libnetStackPosix_CPPFLAGS=\
	-I../include\
	-I../../libkernelTypes\
	-I../../libnet/include

libnetStackPosix_LDFLAGS=\
	-L../libnet

include $(MAKEDIR)/sugarBottom.mk
