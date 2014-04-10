export U ?= $(shell pwd)/../../..

include $(U)/mk/bootstrap.mk

include $(MAKEDIR)/userspace.mk
include $(MAKEDIR)/sugarTop.mk

gopkg_DUALGOLIBRARIES=elScript

elScript_TYPES=elScript.t

elScript_TYPEHASHES=\
	/user/mike/scripts:ElScript \
	/user/mike/tests:ElScript

elScript_GOPKGDIRS=\
	$(U)/../etnTools/types/package/ethos \
	.

noinst_GOPROGRAMS=instElScripts

instElScripts_GOSRC=install.go

instElScripts_GOPKGDIRS=.

# instElScripts_INSTALL_TARGET=instElScripts_inst

install:
	rm -rf $(DESTDIR)/var/lib/ethos/$(ETHOSNAME)/rootfs/user/mike/scripts/*
	./objs-$(TARGET_ARCH)/instElScripts --in ../../../scripts --out $(DESTDIR)/var/lib/ethos/$(ETHOSNAME)/rootfs/user/mike/scripts
	rm -rf $(DESTDIR)/var/lib/ethos/$(ETHOSNAME)/rootfs/user/mike/tests/*
	./objs-$(TARGET_ARCH)/instElScripts --in ../../../tests --out $(DESTDIR)/var/lib/ethos/$(ETHOSNAME)/rootfs/user/mike/tests

include $(MAKEDIR)/sugarBottom.mk
