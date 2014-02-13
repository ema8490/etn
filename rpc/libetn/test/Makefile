U ?= $(shell pwd)/..

include $(U)/mk/bootstrap.mk
include $(MAKEDIR)/sugarTop.mk

noinst_PROGRAMS = \
	control \
	controlClient \
	controlServer \
	runUnitTests \
	rpcDemoClient \
	rpcDemoServer \
	runBenchmarks \
	runBenchmarksClient \
	runBenchmarksServer \
	#tortureClient \
	tortureServer \
	handCraftedEncode \

noinst_LIBRARIES = \
	libtestGraph

rpcDemoClient_OBJS = rpcDemoClient.o rpcDemoCommon.o unitTest.o
rpcDemoClient_CPPFLAGS = -I../include
rpcDemoClient_LDFLAGS = -L. -L../lib
rpcDemoClient_LIBS = testGraph etn adt debug fmt utf core mixin xmalloc xalloc fmt mixin

rpcDemoServer_OBJS = rpcDemoServer.o rpcDemoCommon.o unitTest.o
rpcDemoServer_CPPFLAGS = -I../include
rpcDemoServer_LDFLAGS = -L. -L../lib
rpcDemoServer_LIBS = testGraph etn adt debug fmt utf core mixin xmalloc xalloc fmt mixin

handCraftedEncode_OBJS = handCraftedEncode.o unitTest.o
handCraftedEncode_CPPFLAGS = -I../include
handCraftedEncode_LDFLAGS = -L. -L../lib
handCraftedEncode_LIBS = testGraph etn adt debug fmt utf core mixin xmalloc xalloc fmt mixin

libtestGraph_OBJS = rpcDemoCommon.o
libtestGraph_CPPFLAGS = -I../include
libtestGraph_LDFLAGS = -L../lib

libtestGraph_TYPES = \
	Buffer.t \
	Certificate.t \
	LoopA.t \
	LoopB.t \
	LoopC.t \
	LoopHead.t \
	RpcBenchmark.t \
	RpcTest.t \
	Strings.t \
	StringAndAny.t \
	StructContainingTuple.t \
	TestArray.t \
	TestUnion.t \
	Torture.t \
	TestTuple.t \
	Tree.t \
	Uints.t

libtestGraph_GRAPHS = \
	../lib/ethos/typeGraph

runUnitTests_OBJS = \
	runUnitTests.o \
	unitTest.o \
	testAdam.o \
	treeFns.o \
	testByteArray.o \
	testString.o \
	testStringEmpty.o \
	testTree.o \
	testTuple.o \
	testTupleEmpty.o \
	testBigString.o \
	testPointer.o \
	testRedBlackTree.o \
	testStructLoop.o \
	testStructContainingTuple.o \
	testUnion.o \
	testAnotherStructLoop.o \
	testPointIntoMiddle.o \
	testTorture.o \
	testUints.o \
	testStrings.o \
	testStringsArray.o \
	testStringAndAny.o \
	testStringAndAnyTuple.o \
	testUint32Array.o \
	testDecodeLibTypeGraph.o \
	testDecodeTestTypeGraph.o \
	testDecodeRandom.o \
	testSizeTorture.o \
	testTupleTuple.o \
	testVerifyByte.o \
	testVerifyArray.o \
	testVerifyStringsArray.o \
	testVerifyString.o \
	testVerifyUints.o \
	testVerifyStrings.o \
	testVerifyStringAndAny.o \
	testVerifyStringAndAnyTuple.o \
	testVerifyUnion.o \
	testVerifyInterface.o \
	testVerifyRandom.o \
	testVerifyTorture.o \

runUnitTests_CPPFLAGS = -I../include
runUnitTests_LDFLAGS = -L. -L../lib
runUnitTests_LIBS = testGraph etn adt debug fmt utf core mixin xmalloc xalloc fmt mixin

runBenchmarks_OBJS = runBenchmarks.o unitTest.o treeFns.o
runBenchmarks_CPPFLAGS = -I../include
runBenchmarks_LDFLAGS = -L. -L../lib
runBenchmarks_LIBS = testGraph etn adt debug fmt utf core mixin xmalloc xalloc fmt mixin

runBenchmarksClient_OBJS = runBenchmarksClient.o unitTest.o treeFns.o
runBenchmarksClient_CPPFLAGS = -I../include
runBenchmarksClient_LDFLAGS = -L. -L../lib
runBenchmarksClient_LIBS = testGraph etn adt debug fmt utf core mixin xmalloc xalloc fmt mixin

runBenchmarksServer_OBJS = runBenchmarksServer.o unitTest.o treeFns.o
runBenchmarksServer_CPPFLAGS = -I../include
runBenchmarksServer_LDFLAGS = -L. -L../lib
runBenchmarksServer_LIBS = testGraph etn adt debug fmt utf core mixin xmalloc xalloc fmt mixin

tortureClient_OBJS =  tortureClient.o unitTest.o
tortureClient_LIBS = testGraph etn adt debug fmt utf core mixin
tortureClient_CPPFLAGS = -I../include
tortureClient_LDFLAGS = -L. -L../lib

tortureServer_OBJS =  tortureServer.o unitTest.o
tortureServer_LIBS = testGraph etn adt debug fmt utf core xmalloc xalloc fmt mixin
tortureServer_CPPFLAGS = -I../include
tortureServer_LDFLAGS = -L. -L../lib

control_OBJS =  control.o
control_CPPFLAGS = -I../include
control_LDFLAGS = -L.

controlClient_OBJS =  controlClient.o

controlServer_OBJS =  controlServer.o






# FIXME: Integrate rpcgen into Ethos build system?
#        Problems: rpcgen generates code that causes compiler warnings.
ifeq ($(TARGET_ARCH),x86_64)
CONTROLCFLAGS=-m64 -I../include -I$(DESTDIR)/$(INCLUDEDIR) -I$(DESTDIR)/$(INCLUDEDIR)/nacl/$(TARGET_ARCH) -I/usr/include/nacl/$(TARGET_ARCH) -I/usr/include/tirpc
CONTROLLDFLAGS=-m64 -L../lib -L/usr/lib64/ethos -L$(DESTDIR)/$(LIBDIR)
else
CONTROLCFLAGS=-m32 -I../include -I$(DESTDIR)/$(INCLUDEDIR) -I$(DESTDIR)/$(INCLUDEDIR)/nacl/$(TARGET_ARCH) -I/usr/include/nacl/$(TARGET_ARCH) -I/usr/include/tirpc
CONTROLLDFLAGS=-m32 -L. -L../lib -L/usr/lib/ethos -L$(DESTDIR)/$(LIBDIR)
endif

ifeq ($(WITH_DEBUG),)
CONTROLCFLAGS+=-O3
else
CONTROLCFLAGS+=-g
endif

controlTypes.h: controlTypes.x
	rpcgen -h -o $@ controlTypes.x

# See Red Hat bug #862318 for explanation of sed.
controlTypes.c: controlTypes.x testGraph.h
	rpcgen -c -o $@ controlTypes.x
	sed -i 's/xdr_quad_t/xdr_int64_t/g' $@
	sed -i 's/xdr_u_quad_t/xdr_int64_t/g' $@

# See Red Hat bug #862318 for explanation of sed.
controlTypesClientStub.c: controlTypes.x
	rpcgen -l -o $@ controlTypes.x
	sed -i 's/xdr_quad_t/xdr_int64_t/g' $@
	sed -i 's/xdr_u_quad_t/xdr_int64_t/g' $@

# See Red Hat bug #862318 for explanation of sed.
controlTypesServerStub.c: controlTypes.x
	rpcgen -m -o $@ controlTypes.x
	sed -i 's/xdr_quad_t/xdr_int64_t/g' $@
	sed -i 's/xdr_u_quad_t/xdr_int64_t/g' $@

controlTypes.o: controlTypes.c
	mkdir -p $(OBJDIR)
	gcc $(CONTROLCFLAGS) -c controlTypes.c -o $(OBJDIR)/$@

runControls.o: runControls.c
	mkdir -p $(OBJDIR)
	gcc $(CONTROLCFLAGS) -c runControls.c -o $(OBJDIR)/$@

xdrTreeFns.o: xdrTreeFns.c
	mkdir -p $(OBJDIR)
	gcc $(CONTROLCFLAGS) -c xdrTreeFns.c -o $(OBJDIR)/$@

runControls: runControls.c controlTypes.h runControls.o xdrTreeFns.o controlTypes.o $(OBJDIR)/unitTest.o $(OBJDIR)/testGraph.o $(OBJDIR)/rpcDemoCommon.o $(OBJDIR)/xdrTreeFns.o
	gcc $(CONTROLLDFLAGS) -o $(OBJDIR)/$@ $(OBJDIR)/runControls.o $(OBJDIR)/controlTypes.o $(OBJDIR)/unitTest.o $(OBJDIR)/testGraph.o $(OBJDIR)/rpcDemoCommon.o $(OBJDIR)/xdrTreeFns.o -letn -ladt -ldebug -lfmt -lutf -lcore -lmixin -lxalloc

runControlsClient: runControlsClient.c controlTypesClientStub.c controlTypes.h controlTypes.c $(OBJDIR)/unitTest.o $(OBJDIR)/testGraph.o $(OBJDIR)/rpcDemoCommon.o $(OBJDIR)/xdrTreeFns.o
	gcc $(CONTROLCFLAGS) $(CONTROLLDFLAGS) -o $(OBJDIR)/$@ runControlsClient.c controlTypesClientStub.c controlTypes.c $(OBJDIR)/unitTest.o $(OBJDIR)/testGraph.o $(OBJDIR)/rpcDemoCommon.o $(OBJDIR)/xdrTreeFns.o -letn -ladt -ldebug -lfmt -lutf -lcore -lmixin -lxalloc -ltirpc

runControlsServer: runControlsServer.c controlTypesServerStub.c controlTypes.h controlTypes.c
	gcc $(CONTROLCFLAGS) $(CONTROLLDFLAGS) -DRPC_SVC_FG -o $(OBJDIR)/$@ runControlsServer.c controlTypesServerStub.c controlTypes.c -ltirpc

ALL += runControls runControlsClient runControlsServer
CLEANFILES += \
	runControls \
	runControls.o \
	runControlsClient \
	runControlsClient.o \
	controlTypesClientStub.{c,o} \
	runControlsServer \
	runControlsServer.o \
	controlTypesServerStub.{c,o} \
	controlTypes.{c,h,o}





include $(MAKEDIR)/sugarBottom.mk
