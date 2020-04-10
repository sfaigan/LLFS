CC := gcc
CFLAGS := -g -Wall -ggdb3 -Wno-deprecated-declarations -Werror

TESTS := test01 test02

all: $(TESTS)

clean:
	rm -rf $(TESTS) *.dSYM

test%: test%.c ../io/File.c ../io/File.h ../io/typedefs.h ../io/CONSTANTS.h ../io/util.c ../io/util.h ../disk/driver.c ../disk/driver.h
	$(CC) $(CFLAGS) -I.. -o $@ $< ../io/File.c ../io/util.c ../disk/driver.c -lm
