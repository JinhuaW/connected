SRCS := $(shell ls *.c)
SRCS += ../../lib/conu.c ../../lib/log.c ../../lib/hash.c
OBJS := $(SRCS:%.c=%.o)
CFLAGS += -I ../../lib/include/ -lpthread

PROGRAM=demo_app

all: $(PROGRAM)

clean:
	@rm -rf $(OBJS)
	@rm -rf $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC)  -o $@ $^ $(CFLAGS)
