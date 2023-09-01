#!/bin/bash

# A note on negatives when inputting hex
# --------------------------------------
# We treat the hex input as if the user is intending the data to be the internal storage of the number.
# For example, 0xfe, numerically is the decimal number 254.  So is 0x00fe.
# However, since our intention is that this number will be formatted to the bitwise specification of an
# integer, if we format 0xfe as a int8 then it is the number -2, not 254.  If you try to input 254 as an
# int8 it will fail with a range overflow.
# Further, the leading zero's play a role.  As an int8, 0x00fe signals the user intention that this number
# is really 254, not -2, whereas 0xfe (the precise width of the type) is intended to be -2.
# Lastly, it would be really strange to input negative hex numbers.
# i.e. -0xfe.  That doesn't follow the idea of supporting the internal storage of the number.
# Right now I don't check for this, but it will probably try to format this as -254

echo "Test elim duplicate format types"
./fmttool -i 8 -i 8 99
echo
echo "Test signed 8 bit int"
./fmttool -i 8 -129 -128 127 128 0x80 0x7f 0xfe 0x00fe
echo
echo "Test unsigned 8 bit int"
./fmttool -u 8 -1 0 255 256 0x80
echo "Test signed 16 bit int"
./fmttool -i 16 -32769 -32768 32767 32768 0x8000 0x00008000
echo
echo "Test unsigned 16 bit int"
./fmttool -u 16 -1 0 65535 65536 0x8000 0x00008000
echo
echo "Test signed 32 bit int"
./fmttool -i 32 -2147483649 -2147483648 2147483647 2147483648 0x80000000
echo
echo "Test unsigned 32 bit int"
./fmttool -u 32 -1 0 4294967295 4294967296 0x80000000
echo
echo "Test signed 64 bit int"
./fmttool -i 64 -9223372036854775809 -9223372036854775808 9223372036854775807 9223372036854775808 0x8000000000000000
echo
echo "Test unsigned 64 bit int"
./fmttool -u 64 -1 0 18446744073709551615 18446744073709551616 0x8000000000000000
echo
