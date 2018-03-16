
clang-flags = -Werror -Weverything -Wno-sign-conversion -pedantic -O0 -g -Wno-weak-vtables -Wno-padded -Wno-c++98-compat -Wno-zero-as-null-pointer-constant -Wno-reserved-id-macro -Wno-unused-parameter -Wno-missing-field-initializers -Wno-missing-prototypes

# 
.PHONY: clean all

all: python/pxnd/_pxnd.so

clean:
	-rm -r libplasma/plasma.o libplasma/libplasma.a libplasma/libplasma.so libpxnd/pxnd.o libpxnd/libpxnd.a python/pxnd/_pxnd.so python/pxnd/_pxnd.so.dSYM

arrow/cpp:
	git clone https://github.com/apache/arrow.git
	cd arrow; git checkout apache-arrow-0.8.0

arrow/release: | arrow/cpp
	mkdir -p arrow/release

arrow/release/Makefile: arrow/cpp | arrow/release
	cd arrow/release; cmake ../cpp -DCMAKE_BUILD_TYPE=Release -DARROW_PLASMA=ON

arrow/release/release: arrow/release/Makefile
	cd arrow/release; make unittest || true

libplasma/plasma.o: libplasma/plasma.cc libplasma/plasma.h
	cd libplasma; clang++ -std=c++11 $(clang-flags)  -c -I ../arrow/cpp/src/ plasma.cc

libplasma/libplasma.a: libplasma/plasma.o arrow/release/release
	ar rcs libplasma/libplasma.a libplasma/plasma.o arrow/release/release/libplasma.a arrow/release/release/libarrow.a

xnd:
	git clone https://github.com/plures/xnd.git

xnd/ndtypes: | xnd
	cd xnd; git clone https://github.com/plures/ndtypes.git

xnd/ndtypes/Makefile: | xnd/ndtypes
	cd xnd/ndtypes; ./configure

xnd/ndtypes/libndtypes/libndtypes.a: | xnd/ndtypes/Makefile
	cd xnd/ndtypes; make

xnd/Makefile: | xnd
	cd xnd; ./configure

xnd/libxnd/libxnd.a: xnd/Makefile xnd/ndtypes/libndtypes/libndtypes.a
	cd xnd; make

libpxnd/pxnd.o: libpxnd/pxnd.h libpxnd/pxnd.c xnd/libxnd xnd/ndtypes/libndtypes libplasma
	cd libpxnd && clang $(clang-flags) -std=c11 -c pxnd.c -I ../xnd/libxnd -I ../xnd/ndtypes/libndtypes -I ../libplasma

libpxnd/libpxnd.a: libplasma/libplasma.a libpxnd/pxnd.o xnd/libxnd/libxnd.a
	ar rcs $@ $^

python/pxnd/_pxnd.so: python/_pxnd.c libpxnd/pxnd.o libpxnd/libpxnd.a libpxnd/pxnd.h
	clang  $(shell python-config --includes) $(shell python-config --ldflags) $(clang-flags) -std=c11 -fno-common -dynamic -DNDEBUG -g -fwrapv -I libpxnd -L libpxnd -I xnd/ndtypes/libndtypes -I xnd/libxnd -I libplasma -l pxnd -shared -o $@ python/_pxnd.c

