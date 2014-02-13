ifeq ($(TARGET_ARCH),)
ifeq ($(shell pkg-config --list-all | awk '{ print $$1 }' | grep ethos-x86_64),ethos-x86_64)
TARGET_ARCH = x86_64
else
ifeq ($(shell pkg-config --list-all | awk '{ print $$1 }' | grep ethos-x86_32),ethos-x86_32)
TARGET_ARCH = x86_32
else
TARGET_ARCH = x86_64
endif
endif
endif

BINDIR        ?= $(shell pkg-config --variable=bindir ethos-$(TARGET_ARCH))
DATAROOTDIR   ?= $(shell pkg-config --variable=edatarootdir ethos-$(TARGET_ARCH))
MAKEDIR       ?= $(shell pkg-config --variable=makedir ethos-$(TARGET_ARCH))
# Fallback to mk in build tree if mk not yet installed.
MAKEDIR       := $(shell if [ -d "$(MAKEDIR)" ]; then echo $(MAKEDIR); else echo $(U)/../../mk; fi)
EINCLUDEDIR   ?= $(shell pkg-config --variable=eincludedir ethos-$(TARGET_ARCH))
ELIBDIR       ?= $(shell pkg-config --variable=elibdir ethos-$(TARGET_ARCH))
INCLUDEDIR    ?= $(shell pkg-config --variable=includedir ethos-$(TARGET_ARCH))
LIBDIR        ?= $(shell pkg-config --variable=libdir ethos-$(TARGET_ARCH))
LOCALSTATEDIR ?= $(shell pkg-config --variable=localstatedir ethos-$(TARGET_ARCH))
SBINDIR       ?= $(shell pkg-config --variable=sbindir ethos-$(TARGET_ARCH))

CFLAGS  += -I$(INCLUDEDIR)

# If we are using DESTDIR, then we may have installed dependencies there.
ifneq ($(DESTDIR),)
CPPFLAGS += \
	-I$(DESTDIR)/$(EINCLUDEDIR) \
	-I$(DESTDIR)/$(EINCLUDEDIR)/nacl
endif

LDFLAGS += $(ARCH_LDFLAGS) -L$(LIBDIR)

# If we are using DESTDIR, then we may have installed dependencies there.
ifneq ($(DESTDIR),)
LDFLAGS += \
	-L$(DESTDIR)/$(LIBDIR) \
	-L$(DESTDIR)/$(ELIBDIR)
endif
