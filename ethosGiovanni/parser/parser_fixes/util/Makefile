export U ?= $(shell pwd)/../../..

include $(U)/mk/bootstrap.mk

include $(MAKEDIR)/userspace.mk
include $(MAKEDIR)/sugarTop.mk

noinst_EGOLIBRARIES=util

util_GOSRC=util.go

include $(MAKEDIR)/sugarBottom.mk