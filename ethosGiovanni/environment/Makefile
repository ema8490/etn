export U ?= $(shell pwd)/../../..

include $(U)/mk/bootstrap.mk

include $(MAKEDIR)/userspace.mk
include $(MAKEDIR)/sugarTop.mk

noinst_EGOLIBRARIES=environment

environment_GOSRC=environment.go

environment_GOPKGDIRS=\
	$(U)/src/ethos/parser \
	$(U)/src/ethos/util

include $(MAKEDIR)/sugarBottom.mk