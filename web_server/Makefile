SRCS := $(shell ls *.c)
OBJS := $(SRCS:%.c=%.o)

PROGRAM=exchange
LIBS += -lx -lpthread

all: $(PROGRAM)

clean:
	@rm -rf $(OBJS)
	@rm -rf $(PROGRAM)
%.o:%.c
	$(CROSS_COMPILE)gcc $(CFLAGS) -c  $< -o $@

$(PROGRAM): $(OBJS)
	$(CROSS_COMPILE)gcc $(LIBPATH) -o $@ $^ $(LIBS)
install:
	sudo cp $(PROGRAM) index.html action.php $(HTML_INST_ROOT)
