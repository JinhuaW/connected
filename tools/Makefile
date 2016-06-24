SRCS := echo.c
OBJS := $(SRCS:%.c=%.o)

PROGRAM=echo
LIBS += -lx -lpthread

all: $(PROGRAM)


echo.o:echo.c
	$(CROSS_COMPILE)gcc $(CFLAGS) -c  $< -o $@

$(PROGRAM): $(OBJS)
	$(CROSS_COMPILE)gcc $(LIBPATH) -o $@ $^ $(LIBS)

clean:
	@rm -rf $(OBJS)
	@rm -rf $(PROGRAM)
