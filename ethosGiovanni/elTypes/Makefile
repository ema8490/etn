export U ?= $(shell pwd)/../../..

include $(U)/mk/bootstrap.mk

include $(MAKEDIR)/userspace.mk
include $(MAKEDIR)/sugarTop.mk

gopkg_EGOLIBRARIES=\
	typeDefs \
	elTypes

typeDefs_TYPES=typeDefs.t

elTypes_TYPEHASHES=\
	/user/mike/elPlayground/ints:ElInt \
	/user/mike/elPlayground/floats:ElFloat \
	/user/mike/elPlayground/strings:ElString \
	/user/mike/elPlayground/tuples:ElTuple \
	/user/mike/elP/strings:string \
	/user/mike/elP/ints:int64 \
	/user/mike/elP/bools:bool \
	/user/mike/elP/tuples:[]string \
	/user/mike/elP/tt:TestTuple \
	/user/mike/elP/msg:Message \
	/services/foo:Message \
	/services/foos:string

elTypes_GOSRC=\
	elTypes.go \
	lattice.go

elTypes_GOPKGDIRS= \
	$(U)/src/ethos/util \
	$(U)/../etnTools/types/package/ethos \
	.

include $(MAKEDIR)/sugarBottom.mk