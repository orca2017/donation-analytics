#!/bin/bash
#
# compile program
echo compiling program...
g++ -std=c++11 -O2 -g -o main ./src/main.cpp ./src/Utils.cpp
#
# run program
echo running program...
#./main itcont.txt percentile.txt output.txt
./main ./input/itcont.txt ./input/percentile.txt ./output/repeat_donors.txt
