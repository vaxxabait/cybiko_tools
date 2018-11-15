#!/bin/sh

gcc -c cyusb.c
gcc -c test_cyusb.c
gcc cyusb.o test_cyusb.o -lusb-1.0 -o test_cyusb