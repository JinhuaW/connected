SRCS := $(shell ls *.c)
OBJS := $(SRCS:%.c=%.o)

CFLAGS += -I../include

all: $(OBJS)

clean:
	@rm -rf $(OBJS)
