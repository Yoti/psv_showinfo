#!/bin/sh
if [ -d build ]; then
	rm -rf build
fi
mkdir build && cd build && cmake .. && make && cd ..
if ! [ -d build ]; then
	cd ..
	rm -rf build
fi