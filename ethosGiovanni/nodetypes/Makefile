export U ?= $(shell pwd)/../../..

include $(U)/mk/bootstrap.mk

include $(MAKEDIR)/userspace.mk
include $(MAKEDIR)/sugarTop.mk

noinst_EGOLIBRARIES=nodetypes

nodetypes_GOSRC=nodetypes.go

include $(MAKEDIR)/sugarBottom.mk