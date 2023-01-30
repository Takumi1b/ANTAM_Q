#!/bin/bash
#
REV=unknown
if [ "$(uname)" == 'Darwin' ]; then
  clang -fPIC -shared -Wall -Wno-main -Wno-sign-compare -Wshadow -Wcast-align -Wpointer-arith -Wswitch -Wredundant-decls -Wreturn-type -Wshadow -Wunused -Wl,-install_name,dxlib_m.so.4 -O3 -D__MAKE_LIB__ dxlib_intuitive.cpp dxlib.cpp -o dxlib.so.4.6
else
  clang -fPIC -shared -Wall -Wno-main -Wno-sign-compare -Wshadow -Wcast-align -Wpointer-arith -Wswitch -Wredundant-decls -Wreturn-type -Wshadow -Wunused -Wl,-soname=dxlib_m.so.4 -O3 -D__MAKE_LIB__ dxlib_intuitive.cpp dxlib.cpp -o dxlib.so.4.6
fi

if [ $? -ne 0 ]; then
    echo "Oops..."
    exit 1
fi
rm -f ../SampleCode/Python/dxlib.so.4.6
rm -f ../SampleCode/Python/dxlib.py
cp dxlib.so.4.6 ../SampleCode/Python
cp dxlib.py ../SampleCode/Python

clang -fPIC -shared -Wall -Wno-main -Wno-sign-compare -Wshadow -Wcast-align -Wpointer-arith -Wswitch -Wredundant-decls -Wreturn-type -Wshadow -Wunused -o libdxlib.a -O3 -D__MAKE_LIB__ dxlib_intuitive.cpp dxlib.cpp
if [ $? -ne 0 ]; then
    echo "Oops..."
    exit 1
fi
rm -f ../SampleCode/C/libdxlib.a
rm -f ../SampleCode/C/dxlib.h
rm -f ../SampleCode/C/dxmemmap.h
cp libdxlib.a ../SampleCode/C
cp dxlib.h ../SampleCode/C
cp dxmemmap.h ../SampleCode/C

