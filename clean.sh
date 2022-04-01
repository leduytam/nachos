#!/bin/bash

cd NachOS-4.0/code/build.linux/
make clean && make distclean

cd ../../coff2noff/
make clean && make distclean

cd ../code/test/
make clean && make distclean

cd ../../../
