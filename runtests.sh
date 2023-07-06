#!/bin/bash

echo "Test signed 8 bit int"
./fmttool -i 8 -129 -128 127 128
echo
echo "Test unsigned 8 bit int"
./fmttool -u 8 -1 0 255 256
echo "Test signed 16 bit int"
./fmttool -i 16 -32769 -32768 32767 32768
echo
echo "Test unsigned 16 bit int"
./fmttool -u 16 -1 0 65535 65536
echo
echo "Test signed 32 bit int"
./fmttool -i 32 -2147483649 -2147483648 2147483647 2147483648
echo
echo "Test unsigned 32 bit int"
./fmttool -u 32 -1 0 4294967295 4294967296
echo
echo "Test signed 64 bit int"
./fmttool -i 64 -9223372036854775809 -9223372036854775808 9223372036854775807 9223372036854775808
echo
echo "Test unsigned 64 bit int"
./fmttool -u 64 -1 0 18446744073709551615 18446744073709551616
echo
