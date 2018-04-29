#!/bin/sh

# remove extra spaces
sed -rb 's/ {2,}//g' $1 |
# remove line comments
sed -rb 's/\/\/.*$//g' |
# remove comments
sed -rb 's/\/\*.*\*\///g' |
# remove empty lines
sed -b '/^$/d'
