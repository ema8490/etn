U ?= $(shell pwd)/../..

include $(U)/mk/bootstrap.mk
include $(MAKEDIR)/sugarTop.mk

# NOTE: This has to be in the directory "ethos" because libetn
#       uses #include <ethos/typeGraph.h>. So typeGraph.h has
#       to be in the right spot relative to the libetn sources.

lib_DUALLIBRARIES=libtypeGraph

include_HEADERS=typeGraph.h

libtypeGraph_TYPES = \
        Annotation.t \
        TypeNode.t

libtypeGraph_CPPFLAGS = -I../../include

include $(MAKEDIR)/sugarBottom.mk
