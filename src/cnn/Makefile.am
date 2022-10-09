AM_LDFLAGS = -ljpeg -lm -lpng
# use asan options :
# -fsanitize=address  -fsanitize=undefined 
AM_CFLAGS=-g -fsanitize=address  -fsanitize=undefined
bin_PROGRAMS=cnn
cnn_SOURCES=img.c imgfam.c digits.c grid.c util.c fontname.c main.c filter.c filterfam.c layer.c
#layer3_SOURCES=layer3.c fontname.c

fontname.c : gen_fontlist.sh
	bash gen_fontlist.sh

l3.c : digits
	./digits --create 2
	./digits --create 3

install-data-local :
	mkdir -p @datarootdir@/digits
	mkdir -p @datarootdir@/grid
	bash ./gen_patterns.sh
	rm -f grid && ln -s cnn grid
	rm -f digits && ln -s cnn digits
	rm -f img && ln -s cnn img
	rm -f @prefix@/bin/img && ln -s @prefix@/bin/cnn @prefix@/bin/img
	rm -f @prefix@/bin/digits && ln -s @prefix@/bin/cnn @prefix@/bin/digits
	rm -f @prefix@/bin/grid && ln -s @prefix@/bin/cnn @prefix@/bin/grid
	./digits --create 1
	./digits --create 2

#	cp -r ../share/nn @datarootdir@
#        mkdir -p @datarootdir@/nn/scripts
##        cp -r ../scripts/packages @datarootdir@/tis/scripts

#make
#./digits -c 3
#gcc -c l3.c


clean-local:
	rm -rf html latex img fontname.c fontname.h l3.c digits img grid *~ *.png

