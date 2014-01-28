U ?= $(shell pwd)/../../..

include $(U)/mk/bootstrap.mk
include $(MAKEDIR)/sugarTop.mk
include $(U)/mk/localtools.mk

noinst_DUALGOLIBRARIES=etn

etn_GOSRC=\
	builtins.go\
	doc.go\
	etn.go\
	rpc.go\
	trie.go\
	tt.go

etn_GOPKGDIRS = ../../../types/package/ethos/

include $(MAKEDIR)/sugarBottom.mk
