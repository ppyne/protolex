ROOT := $(shell pwd)

.PHONY: all test clean

all:
	$(MAKE) -C src

test: all
	./tests/run.sh

clean:
	$(MAKE) -C src clean
