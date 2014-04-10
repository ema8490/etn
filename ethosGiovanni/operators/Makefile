U ?= $(shell pwd)/../../..

include $(U)/mk/bootstrap.mk

include $(MAKEDIR)/userspace.mk
include $(MAKEDIR)/sugarTop.mk

noinst_EGOLIBRARIES=operators

operators_GOSRC=\
	and.go \
	div.go \
	equal.go \
	greater.go \
	greaterEqual.go \
	lower.go \
	lowerEqual.go \
	mul.go \
	not.go \
	notEqual.go \
	or.go \
	sub.go \
	sum.go \
	unaryMinus.go \
	operators.go

operators_GOPKGDIRS=\
	$(U)/src/ethos/util \
	$(U)/src/ethos/io \
	$(U)/src/ethos/parser \
	$(U)/src/ethos/elTypes

include $(MAKEDIR)/sugarBottom.mk