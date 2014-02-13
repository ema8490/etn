U ?= $(shell pwd)

include $(U)/mk/bootstrap.mk
include $(MAKEDIR)/sugarTop.mk

include_DUALHEADERS = \
	kernelTypes.h \
	rpcKernel.h \

lib_DUALLIBRARIES = \
	libkernelTypes \
	librpcKernel \

libkernelTypes_TYPES = \
	Account.t \
	Certificate.t \
	DirectoryServiceRecord.t \
	EncryptionKeyPair.t \
	EphemeralHostCertificate.t \
	EthernetMac.t \
	ExitCode.t \
	FileInformation.t \
	Hash.t \
	IpAddress.t \
	PrivateEncryptionKey.t \
	PublicEncryptionKey.t \
	PrivateSignatureKey.t \
	PublicSignatureKey.t \
	Puzzle.t \
	Signature.t \
	SignatureKeyPair.t \
	Status.t \
	Time.t \
	UdpPort.t \
	Authenticator.t \
	UserRecord.t \
	UserCertificate.t \

librpcKernel_TYPES = \
	$(libkernelTypes_TYPES) \
	RpcMinimaLT.t \
	RpcShadowDaemon.t \
	RpcTerminal.t\
	RpcDirectoryCache.t \

librpcKernel_OBJS = \
	rpcDirectoryCacheEmpty.o \
	rpcMinimaLTEmpty.o \
	rpcShadowDaemonEmpty.o \
	rpcTerminalEmpty.o \

librpcKernel_CPPFLAGS = -I../libxalloc/include/ -I../libcore/include/ -I../libetn/include/

include $(MAKEDIR)/sugarBottom.mk
