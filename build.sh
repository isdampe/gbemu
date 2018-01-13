#!/bin/bash
mkdir -p bin
cd src
g++ -std=c++11 -o ../bin/gbemu *.cpp
cd ../
