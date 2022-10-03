bin_PROGRAMS=digits layer3
digits_SOURCES=img.c imgfam.c digits.c util.c fontname.c 
layer3_SOURCES=layer3.c fontname.c

fontname.c : gen_fontlist.sh
	bash gen_fontlist.sh

l3.c : digits
	./digits --create 2
	./digits --create 3

install-data-local :
	mkdir -p @datarootdir@/digits
	bash ./gen_patterns.sh
	./digits --create 2

#	cp -r ../share/nn @datarootdir@
#        mkdir -p @datarootdir@/nn/scripts
##        cp -r ../scripts/packages @datarootdir@/tis/scripts

#make
#./digits -c 3
#gcc -c l3.c


clean-local:
	rm -rf html latex img fontname.c l3.c fontname.h


