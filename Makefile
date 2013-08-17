
all: dnsmux
.PHONY : all

dnsmux:
	make -C src dnsmux
.PHONY : dnsmux

debug:
	make -C src debug
.PHONY : debug

clean:
	make -C src clean
.PHONY : clean

