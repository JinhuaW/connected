SRCS := $(shell ls *.c)
OBJS := $(SRCS:%.c=%.o)

PROGRAM=echo calam remote_exec
LIBS += -lx -lpthread

all:echo calam remote_exec


%.o:%.c
	$(CROSS_COMPILE)gcc $(CFLAGS) -c  $< -o $@

echo:echo.o
	$(CROSS_COMPILE)gcc $(LIBPATH) -o $@ $^ $(LIBS)

calam:calam.o
	$(CROSS_COMPILE)gcc $(LIBPATH) -o $@ $^ $(LIBS)

remote_exec:remote_exec.o
	$(CROSS_COMPILE)gcc $(LIBPATH) -o $@ $^ $(LIBS)

clean:
	@rm -rf $(OBJS)
	@rm -rf $(PROGRAM)
install:
	cp $(PROGRAM) $(TOPDIR)/output
