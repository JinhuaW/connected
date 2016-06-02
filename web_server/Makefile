SRCS := $(shell ls *.c)
OBJS := $(SRCS:%.c=%.o)

PROGRAM=web_server

all: $(PROGRAM)

clean:
	@rm -rf $(OBJS)
	@rm -rf $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC)  -o $@ $^
