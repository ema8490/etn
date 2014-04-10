export U ?= $(shell pwd)/../../..

include $(U)/mk/bootstrap.mk

include $(MAKEDIR)/userspace.mk
include $(MAKEDIR)/sugarTop.mk

bin_EGOPROGRAMS=el

el_GOSRC=el.go

el_GOPKGDIRS= \
	$(U)/src/ethos/util \
	$(U)/src/ethos/environment \
	$(U)/src/ethos/operators \
	$(U)/src/ethos/eval \
	$(U)/src/ethos/nodetypes \
	$(U)/src/ethos/parser \
	$(U)/src/ethos/elTypes \
	$(U)/src/ethos/io \
	$(U)/src/ethos/elScript \
	$(U)/../etnTools/types/package/ethos \
	$(U)/../etnTools/etn/package/ethos \
	.

include $(MAKEDIR)/sugarBottom.mk
