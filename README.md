A tool for formatting numbers and stuff.

For example, here's a test run that wants to format the inputs as 16-bit signed integers.  Have a look at the test script to see different examples.

./fmttool -i 16 -32769 -32768 32767 32768 0x8000 0x00008000

                   Base 10             Hex               Bin  
     input         int16_t         int16_t           int16_t  
----------  --------------  --------------  ----------------  
    -32769  <out_of_range>  <out_of_range>    <out_of_range>  
    -32768          -32768          0x8000  1000000000000000  
     32767           32767          0x7fff  0111111111111111  
     32768  <out_of_range>  <out_of_range>    <out_of_range>  
    0x8000          -32768          0x8000  1000000000000000  
0x00008000  <out_of_range>  <out_of_range>    <out_of_range>

A note on negatives when inputting hex
--------------------------------------
We treat the hex input as if the user is intending the data to be the internal storage of the number.
For example, 0xfe, numerically is the decimal number 254.  So is 0x00fe.
However, since our intention is that this number will be formatted to the bitwise specification of an
integer, if we format 0xfe as a int8 then it is the number -2, not 254.  If you try to input 254 as an
int8 it will fail with a range overflow.
Further, the leading zero's play a role.  As an int8, 0x00fe signals the user intention that this number
is really 254, not -2, whereas 0xfe (the precise width of the type) is intended to be -2.
Lastly, it would be really strange to input negative hex numbers.
i.e. -0xfe.  That doesn't follow the idea of supporting the internal storage of the number.
Right now I don't check for this, but it will probably try to format this as -254it push -u origin main
