SRCS := $(shell ls *.c)
OBJS := $(SRCS:%.c=%.o)

COMMON_OBJS += ../common/command.o

LIBS += -lx


PROGRAM=yunuo

all: $(PROGRAM)

clean:
	@rm -rf $(OBJS)
	@rm -rf $(PROGRAM)

$(PROGRAM): $(OBJS) $(COMMON_OBJS) 
	$(CC) $(LIBPATH) -o $@ $^ $(LIBS)
