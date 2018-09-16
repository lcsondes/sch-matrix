#!/bin/sh

mkdir build
for i in qtiocompressor mnp4 animeditor mxclient mxserver q4xtract; do
mkdir build/$i
mkdir build/$i/obj
mkdir build/$i/moc
mkdir build/$i/uic
mkdir build/$i/rcc
done