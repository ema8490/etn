export U ?= $(shell pwd)/../../..

include $(U)/mk/bootstrap.mk

include $(MAKEDIR)/userspace.mk
include $(MAKEDIR)/sugarTop.mk

noinst_EGOLIBRARIES=parser

parser_GOYACCSRC=parser.y

parser_GOSRC=\
	lexer.go \
	parsetree.go

parser_GOPKGDIRS=\
	$(U)/src/ethos/nodetypes \
	$(U)/src/ethos/util

include $(MAKEDIR)/sugarBottom.mk