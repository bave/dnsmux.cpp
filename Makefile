
all: dnsmux test
.PHONY : all

dnsmux:
	gmake -C src dnsmux
.PHONY : dnsmux

debug:
	gmake -C src debug
.PHONY : debug

test:
	gmake -C test
.PHONY : test

clean:
	gmake -C src clean
	gmake -C test clean
.PHONY : clean

