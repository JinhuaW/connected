SRCS := server.c hash.c
OBJS := $(SRCS:%.c=%.o)

CFLAGS += -I$(TOPDIR)/lib/include
LIBPATH += -L$(TOPDIR)/lib

LIBS += -lpthread -lx

PROGRAM=proxy_server

all: $(PROGRAM)

clean:
	@rm -rf $(OBJS)
	@rm -rf $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(LIBPATH) -o $@ $^ $(LIBS)
