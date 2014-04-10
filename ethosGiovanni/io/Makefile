export U ?= $(shell pwd)/../../..

include $(U)/mk/bootstrap.mk

include $(MAKEDIR)/userspace.mk
include $(MAKEDIR)/sugarTop.mk

gopkg_EGOLIBRARIES=io

io_GOSRC=\
	io.go \
	typed.go \
	mapping.go

io_GOPKGDIRS=\
	$(U)/src/ethos/util \
	$(U)/src/ethos/elTypes \
	$(U)/src/ethos/parser \
	$(U)/src/ethos/operators \
	$(U)/src/ethos/elScript \
	$(U)/../../goGenericPackages/kernelTypes \
	$(U)/../../languages/etnTools/types/package/ethos \
	$(U)/../../programs/tools/msg \
	$(U)/../../languages/etnTools/etn/package/ethos \
	.

io_EGOPKGDIRS=\
	$(U)/../../languages/etnTools/etn/package/ethos \
	.

include $(MAKEDIR)/sugarBottom.mk
