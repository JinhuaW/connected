ARCH          ?=
CROSS_COMPILE ?=

TOPDIR=$(PWD)
export TOPDIR

include .config

all:
	$(MAKE) all -e -C lib 
	$(MAKE) all -e -C boards/$(CONFIG_BOARD)
	$(MAKE) all -e -C proxy_server
	$(MAKE) all -e -C web_server

clean:
	$(MAKE) clean -e -C lib 
	$(MAKE) clean -e -C boards/$(CONFIG_BOARD)
	$(MAKE) clean -e -C proxy_server
	$(MAKE) clean -e -C web_server
