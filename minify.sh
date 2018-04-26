#!/bin/sh
sed -rb 's/ {6}//g' $1 |
sed -rb 's/\/\/.*$//g' |
tr -d '\n' |
sed -rb 's/\/\*.*\*\///g' |
sed -rb 's/(#include.*>)/\1\n/g'
