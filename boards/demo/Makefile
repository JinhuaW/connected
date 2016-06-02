SRCS := $(shell ls *.c)
OBJS := $(SRCS:%.c=%.o)

CFLAGS += -I$(TOPDIR)/proxy_server

PROGRAM=demo_app

all: $(PROGRAM)

clean:
	@rm -rf $(OBJS)
	@rm -rf $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC)  -o $@ $^
