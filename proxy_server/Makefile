SRCS := $(shell ls *.c)
OBJS := $(SRCS:%.c=%.o)

CFLAGS += -I$(TOPDIR)/lib/include

LIBS += -lpthread -lx

PROGRAM=proxy_server

all: $(PROGRAM)

clean:
	@rm -rf $(OBJS)
	@rm -rf $(PROGRAM)

%.o:%.c
	$(CROSS_COMPILE)gcc $(CFLAGS) -c  $< -o $@

$(PROGRAM): $(OBJS)
	$(CROSS_COMPILE)gcc $(LIBPATH) -o $@ $^ $(LIBS)
