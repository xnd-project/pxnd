
clang-flags = -Weverything -pedantic # warn on everything
 # dont warn on external files
clang-flags += \
  --system-header-prefix=_xnd.c \
  --system-header-prefix=xnd.h \
  --system-header-prefix=ndtypes.h \
  --system-header-prefix=arrow/ \
  --system-header-prefix=plasma/ \
  --system-header-prefix=Python.h
clang-flags += -O0 -g # allow debugging



.PHONY: clean all

all: python/pxnd/_pxnd.so

clean:
	-rm -r libplasma/plasma.o \
	libplasma/libplasma.a \
	libplasma/libplasma.so \
	libpxnd/pxnd.o \
	libpxnd/libpxnd.a \
	python/pxnd/_pxnd.so \
	python/pxnd/_pxnd.so.dSYM \
	python/_plasma.o

arrow/cpp arrow/c_glib:
	git clone https://github.com/apache/arrow.git
	cd arrow; git checkout apache-arrow-0.9.0

arrow/release: arrow/cpp
	mkdir -p arrow/release

arrow/release/Makefile: arrow/cpp | arrow/release
	cd arrow/release; cmake ../cpp -DCMAKE_BUILD_TYPE=Release -DARROW_PLASMA=ON
# -DARROW_BOOST_VENDORED=ON

arrow/release/release/libplasma.a arrow/release/release/libarrow.a: arrow/release/Makefile
	cd arrow/release; make unittest || true

arrow/c_glib/configure: | arrow/c_glib
	cd arrow/c_glib && ./autogen.sh

arrow/c_glib/Makefile: arrow/release/release/libarrow.a arrow/cpp arrow/c_glib/configure
	cd arrow/c_glib && ./configure \
		ARROW_CFLAGS="-I$(shell pwd)/arrow/cpp/src/" \
		ARROW_LIBS="-L$(shell pwd)/arrow/release/release -larrow -lboost_regex" \
		PKG_CONFIG_PATH=$$(brew --prefix libffi)/lib/pkgconfig \
		--prefix=$$(pwd)/install

arrow/c_glib/arrow-glib/libarrow-glib.la: arrow/c_glib/Makefile
	cd arrow/c_glib && make

arrow/c_glib/install: arrow/c_glib/arrow-glib/libarrow-glib.la
	cd arrow/c_glib && make V=1 VERBOSE=1 install

libplasma/plasma.o: libplasma/plasma.cc libplasma/plasma.h
	cd libplasma; clang++ \
		-std=c++11 \
		$(clang-flags) \
		-c \
		-I ../arrow/cpp/src/ \
		-I ../arrow/c_glib/release/include \
		plasma.cc

libplasma/plasma.so: libplasma/plasma.cc arrow/release/release/libplasma.a arrow/release/release/libarrow.a
	clang++ -std=c++11 $(clang-flags) -shared -fPIC -o $@ -I ../arrow/cpp/src/ $^


libplasma/libplasma.a: libplasma/plasma.o arrow/release/release/libplasma.a arrow/release/release/libarrow.a
	libtool -static -o $@ $^ -lstdc++

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

libpxnd/pxnd.o: libpxnd/pxnd.h libpxnd/pxnd.c xnd/libxnd xnd/ndtypes/libndtypes libplasma/plasma.h
	cd libpxnd && clang $(clang-flags) \
		-std=c11 \
		-c pxnd.c \
		-I ../xnd/libxnd \
		-I ../xnd/ndtypes/libndtypes \
		-I ../libplasma

libpxnd/libpxnd.a: libpxnd/pxnd.o xnd/libxnd/libxnd.a xnd/ndtypes/libndtypes/libndtypes.a libplasma/libplasma.a
	libtool -static -o $@ $^

python/_plasma.o: python/_plasma.cc python/_plasma.h libplasma/plasma.h
	cd python; clang++ \
		-std=c++11 \
		$(clang-flags) \
		$(shell python-config --includes) \
		-c \
		-I ../arrow/cpp/src/ \
		-I ../libplasma/ \
		_plasma.cc

python/pxnd/_pxnd.so: python/_pxnd.c libpxnd/pxnd.o libpxnd/libpxnd.a libpxnd/pxnd.h libplasma/libplasma.a python/_plasma.o
	clang  -lstdc++ $(shell python-config --includes) $(shell python-config --ldflags) $(clang-flags) -std=c11 -fno-common -dynamic -DNDEBUG -g -fwrapv -I libpxnd -L libpxnd -I xnd/ndtypes/libndtypes -I xnd/libxnd -I xnd/python/xnd -I xnd/ndtypes/python/ndtypes -I libplasma -L libplasma -l plasma -shared -o $@ python/_pxnd.c python/_plasma.o -l pxnd 

