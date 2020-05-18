CFLAGS = -MMD -MP -Werror -Wall -ggdb3 -Wmissing-prototypes -Wwrite-strings
LDLIBS = liblis.a

NULL =

OBJS = \
	abortwithmessage.o \
	closefrom.o \
	fdremap.o \
	$(NULL)

TESTS = \
	test-closefrom \
	$(NULL)

DEPS = $(OBJS:%.o=%.d) $(TESTS:%=%.d)
%: %.c
	$(CC) $(CFLAGS) $< $(LDLIBS) -o $@

all: $(TESTS)

$(TESTS): liblis.a

liblis.a: $(OBJS)
	ar rcs $@ $^

clean:
	rm -f $(OBJS) $(DEPS) $(TESTS) liblis.a

check: $(TESTS)
	set -e; for test in $(TESTS); do echo $$test; ./$$test; done

include $(wildcard $(DEPS))
