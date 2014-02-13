U ?= $(shell pwd)/..

include $(U)/mk/bootstrap.mk
include $(MAKEDIR)/sugarTop.mk

SUBDIRS = ethos

lib_DUALLIBRARIES=libetn

libetn_OBJS= \
	decoder.o\
	encoder.o\
	erpc.o\
	types.o\
	util.o\
	verify.o

libetn_CPPFLAGS = \
	-I../include

# For libtypeGraph, which is generated and so not in normal places.
CPPFLAGS += -I.

# FIXME: Use gnu89 instead of gnu99; gets us compound literals as constant expressions.
#        Ethos build default is -gnu99, but this will follow, overriding.
#        See also: am.mk
#        See also: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=7946
CFLAGS += \
	-std=gnu89

include $(MAKEDIR)/sugarBottom.mk
