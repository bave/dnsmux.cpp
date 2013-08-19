
all: dnsmux test
.PHONY : all

dnsmux:
	make -C src dnsmux
.PHONY : dnsmux

debug:
	make -C src debug
.PHONY : debug

test:
	make -C test
.PHONY : test

clean:
	make -C src clean
	make -C test clean
.PHONY : clean

