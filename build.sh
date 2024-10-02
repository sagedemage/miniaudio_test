#!/bin/sh

g++ main.cpp -ldl -lpthread -lm -msse2 -mavx2 -I include -o bin/out
