#!/bin/sh

compiler="$((g++ -v 2>/dev/null && echo "g++") ||\
            (clang++ -v 2>/dev/null && echo "clang++"))"

${compiler} -std=c++14 example.cpp ../cube.o
