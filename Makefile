CC = gcc
CFLAGS = -Wall -Wextra -O3

# COMPLETE list of renames to match your math.c and the test suite
RENAMES = \
    -Dsin=custom_sin     -Dcos=custom_cos     -Dtan=custom_tan \
    -Dasin=custom_asin   -Dacos=custom_acos   -Datan=custom_atan \
    -Datan2=custom_atan2 \
    -Dsinh=custom_sinh   -Dcosh=custom_cosh   -Dtanh=custom_tanh \
    -Dasinh=custom_asinh -Dacosh=custom_acosh -Datanh=custom_atanh \
    -Dexp=custom_exp     -Dlog=custom_log     -Dlog1p=custom_log1p \
    -Dpow=custom_pow     -Dsqrt=custom_sqrt   -Dfabs=custom_fabs \
    -Dceil=custom_ceil   -Dfloor=custom_floor -Dfmod=custom_fmod \
    -Dtrunc=custom_trunc -Dround=custom_round

all: test_suite

# Compile your custom math.c with ALL renames applied
custom_math.o: math.c
	$(CC) $(CFLAGS) $(RENAMES) -c math.c -o custom_math.o

test_suite.o: test_suite.c
	$(CC) $(CFLAGS) -c test_suite.c -o test_suite.o

test_suite: test_suite.o custom_math.o
	$(CC) -o run_tests test_suite.o custom_math.o -lm

clean:
	rm -f *.o run_tests