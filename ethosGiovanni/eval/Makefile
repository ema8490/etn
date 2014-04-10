export U ?= $(shell pwd)/../../..

include $(U)/mk/bootstrap.mk

include $(MAKEDIR)/userspace.mk
include $(MAKEDIR)/sugarTop.mk

noinst_EGOLIBRARIES=eval

eval_GOSRC=\
	builtin.go \
	map.go \
	string.go \
	tuple.go \
	set.go \
	up.go \
	constructor.go \
	exec.go \
	down.go \
	eval.go

eval_GOPKGDIRS=\
	$(U)/src/ethos/util \
	$(U)/src/ethos/environment \
	$(U)/src/ethos/nodetypes \
	$(U)/src/ethos/io \
	$(U)/src/ethos/operators \
	$(U)/src/ethos/parser \
	$(U)/src/ethos/elTypes

include $(MAKEDIR)/sugarBottom.mk