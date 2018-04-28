#!/bin/sh


# remove extra spaces
sed -rb 's/ {2,}//g' $1 |
# remove line comments
sed -rb 's/\/\/.*$//g' |
# remove newlines
tr -d '\n' |
# remove comments
sed -rb 's/\/\*.*\*\///g' |
# add newline before #include
sed -rb 's/#include/\n#include/g' |
# add newline after >
sed -rb 's/>/>\n/g'
# ^ these work because C doesn't use <> for anything else than include
