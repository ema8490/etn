U ?= $(shell pwd)/../..

include $(U)/mk/bootstrap.mk

include $(MAKEDIR)/userspace.mk
include $(MAKEDIR)/sugarTop.mk

SUBDIRS=\
	util \
	nodetypes \
	elTypes \
	parser \
	environment \
	elScript \
	io \
	operators \
	eval \
	el

include $(MAKEDIR)/sugarBottom.mk
