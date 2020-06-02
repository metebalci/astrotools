
.DEFAULT_GOAL := all

CC=gcc
CFLAGS=-I. -lraw -fopenmp -lm -lcfitsio -Wall
DEPS=common.h

debug: CFLAGS += -g

all: at_raw2fits at_stack at_calc at_stat at_superpixel at_median_filter
debug: at_raw2fits at_stack at_calc at_stat at_superpixel at_median_filter

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

at_raw2fits: raw2fits.o common.o
	$(CC) -o $@ $^ $(CFLAGS)

at_stack: stack.o common.o
	$(CC) -o $@ $^ $(CFLAGS)

at_calc: calc.o common.o
	$(CC) -o $@ $^ $(CFLAGS)

at_stat: stat.o common.o
	$(CC) -o $@ $^ $(CFLAGS)

at_superpixel: superpixel.o common.o
	$(CC) -o $@ $^ $(CFLAGS)

at_median_filter: median-filter.o common.o
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -rf *.o
	find . -name 'at_*' -executable -type f -exec rm -f {} \;
