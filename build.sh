#!/bin/sh
#
# Too lazy to make a makefile, so here we go.

export CLANG=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++

${CLANG} -g -x c++ -std=c++11 -stdlib=libc++ -framework CoreFoundation test.cpp -o ktest
