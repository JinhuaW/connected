ARCH          ?=
CROSS_COMPILE ?=
HTML_INST_ROOT ?= /var/www

TOPDIR=$(PWD)
LIBPATH += -L$(TOPDIR)/lib
CFLAGS += -I. -I$(TOPDIR)/lib/include -I$(TOPDIR)/boards/include

export TOPDIR LIBPATH CFLAGS CROSS_COMPILE HTML_INST_ROOT

-include .config

all:
	$(MAKE) all -e -C lib 
	$(MAKE) all -e -C tools
	$(MAKE) all -e -C proxy_server
	$(MAKE) all -e -C web_server

install:
	$(shell mkdir output)
	$(MAKE) install -e -C tools
	$(MAKE) install -e -C web_server
	$(MAKE) install -e -C proxy_server

clean:
	$(MAKE) clean -e -C lib 
	$(MAKE) clean -e -C proxy_server
	$(MAKE) clean -e -C tools
	$(MAKE) clean -e -C web_server
	-rm -rf output
