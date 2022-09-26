where-am-i = $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
THIS_MAKEFILE := $(call where-am-i)
#$(info $(THIS_MAKEFILE))
ROOT := $(dir $(THIS_MAKEFILE))


all: x86_64_g armhf_g

#armhf_g x86_64
#armel armhf

.PHONY: armhf_g armhf x86_64 x86_64_g

armhf:
	$(MAKE) -f arch.mk ARCH="armhf" ROOT=$(ROOT) OPT="-O2 -g" all

armhf_g:
	$(MAKE) -f arch.mk ARCH="armhf" ROOT=$(ROOT) SUFFIX=_g OPT="-O0 -ggdb3" all

x86_64:
	$(MAKE) -f arch.mk ARCH="x86_64" ROOT=$(ROOT) OPT="-O2 -g" all

x86_64_g:
	$(MAKE) -f arch.mk ARCH="x86_64" ROOT=$(ROOT) SUFFIX=_g OPT="-O0 -ggdb3" all


clean:
	$(MAKE) -f arch.mk ARCH="armhf" ROOT=$(ROOT) clean
	$(MAKE) -f arch.mk ARCH="armhf" ROOT=$(ROOT) SUFFIX=_g clean	
	$(MAKE) -f arch.mk ARCH="x86_64" ROOT=$(ROOT) clean
	$(MAKE) -f arch.mk ARCH="x86_64" ROOT=$(ROOT) SUFFIX=_g clean

