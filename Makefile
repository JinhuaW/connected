ARCH          ?=
CROSS_COMPILE ?=

TOPDIR=$(PWD)
LIBPATH += -L$(TOPDIR)/lib
CFLAGS += -I. -I$(TOPDIR)/lib/include -I$(TOPDIR)/boards/include

export TOPDIR LIBPATH CFLAGS

include .config

all:
	$(MAKE) all -e -C lib 
	$(MAKE) all -e -C tools
	$(MAKE) all -e -C proxy_server
	$(MAKE) all -e -C web_server
	$(MAKE) all -e -C boards/common
	$(MAKE) all -e -C boards/$(CONFIG_BOARD)

clean:
	$(MAKE) clean -e -C lib 
	$(MAKE) clean -e -C boards/common
	$(MAKE) clean -e -C boards/$(CONFIG_BOARD)
	$(MAKE) clean -e -C proxy_server
	$(MAKE) clean -e -C web_server
