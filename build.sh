#!/bin/bash

cd NachOS-4.0/code/build.linux/
make depend
make

cd ../../coff2noff/
make

cd ../code/test/
make

cd ../../../
