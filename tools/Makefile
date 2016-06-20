SRCS := echo.c
OBJS := $(SRCS:%.c=%.o)

PROGRAM=echo
LIBS += -lx -lpthread

all: $(PROGRAM)

clean:
	@rm -rf $(OBJS)
	@rm -rf $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(LIBPATH) -o $@ $^ $(LIBS)
