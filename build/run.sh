#!/bin/bash
# run script for the cpp bsv parser
# must provide path to blocks as object file arg

g++ -std=c++17 ../main.cpp -I /home/supermicro/Documents/omkar/taskflow -O2 -pthread -lstdc++fs -o main.o
./main.o /mnt/nvme2/data/blocks/