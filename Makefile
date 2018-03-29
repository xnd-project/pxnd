
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


# we tell each shared library to just be named by its filename
# so we can copy them into the python distribution and it can
# access the. Like:
# -Wl,-install_name,@loader_path/_plasma.so



.PHONY: clean all

all: python/pxnd/_pxnd.so

clean:
	-rm -vr $(foreach dir,libplasma libpxnd python python/pxnd, $(wildcard $(dir)/*.so $(dir)/*.so.dSYM))

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

# arrow/c_glib/configure: | arrow/c_glib
# 	cd arrow/c_glib && ./autogen.sh

# arrow/c_glib/Makefile: arrow/release/release/libarrow.a arrow/cpp arrow/c_glib/configure
# 	cd arrow/c_glib && ./configure \
# 		ARROW_CFLAGS="-I$(shell pwd)/arrow/cpp/src/" \
# 		ARROW_LIBS="-L$(shell pwd)/arrow/release/release -larrow -lboost_regex" \
# 		PKG_CONFIG_PATH=$$(brew --prefix libffi)/lib/pkgconfig \
# 		--prefix=$$(pwd)/install

# arrow/c_glib/arrow-glib/libarrow-glib.la: arrow/c_glib/Makefile
# 	cd arrow/c_glib && make

# arrow/c_glib/install: arrow/c_glib/arrow-glib/libarrow-glib.la
# 	cd arrow/c_glib && make V=1 VERBOSE=1 install

# libplasma/plasma.o: libplasma/plasma.cc libplasma/plasma.h
# 	cd libplasma; clang++ \
# 		-std=c++11 \
# 		$(clang-flags) \
# 		-c \
# 		-I ../arrow/cpp/src/ \
# 		-I ../arrow/c_glib/release/include \
# 		plasma.cc

python/pxnd/plasma.so: libplasma/plasma.cc arrow/release/release/libplasma.a arrow/release/release/libarrow.a
	clang++ -std=c++11 $(clang-flags) -shared -fPIC -o $@ -I arrow/cpp/src/ -Wl,-install_name,@loader_path/plasma.so $^

# libplasma/libplasma.a: libplasma/plasma.o arrow/release/release/libplasma.a arrow/release/release/libarrow.a
# 	libtool -static -o $@ $^ -lstdc++

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

# libpxnd/pxnd.o: libpxnd/pxnd.h libpxnd/pxnd.c xnd/libxnd xnd/ndtypes/libndtypes libplasma/plasma.h
# 	cd libpxnd && clang $(clang-flags) \
# 		-std=c11 \
# 		-c pxnd.c \
# 		-I ../xnd/libxnd \
# 		-I ../xnd/ndtypes/libndtypes \
# 		-I ../libplasma

# libpxnd/libpxnd.a: libpxnd/pxnd.o xnd/libxnd/libxnd.a xnd/ndtypes/libndtypes/libndtypes.a libplasma/libplasma.a
# 	libtool -static -o $@ $^

python/pxnd/pxnd.so: libpxnd/pxnd.c python/pxnd/plasma.so xnd/libxnd/libxnd.a xnd/ndtypes/libndtypes/libndtypes.a
	clang -std=c11 $(clang-flags) -shared -fPIC -o $@ -I xnd/libxnd -I xnd/ndtypes/libndtypes -I libplasma -Wl,-install_name,@loader_path/pxnd.so $^

python/pxnd/_plasma.so: python/_plasma.cc
	clang++ -std=c++11 \
		$(clang-flags) \
		$(shell python-config --includes) \
		-shared -fPIC \
		-o $@ \
		-I arrow/cpp/src/ \
		-I libplasma \
		-Wl,-install_name,@loader_path/_plasma.so \
		$^

python/pxnd/_pxnd.so: python/_pxnd.c python/pxnd/_plasma.so python/pxnd/_plasma.so python/pxnd/pxnd.so xnd/ndtypes/libndtypes/libndtypes.a xnd/libxnd/libxnd.a 
	clang -std=c11 $(shell python-config --includes) $(shell python-config --ldflags) $(clang-flags) \
		-I libplasma \
		-I xnd/ndtypes/libndtypes \
		-I xnd/libxnd \
		-I libpxnd \
		-I xnd/python/xnd \
		-I xnd/ndtypes/python/ndtypes \
		-shared -fPIC -o $@ $^
