#!/bin/sh

# remove extra spaces, comments and empty lines
FILECONTENTS=`sed -rb 's/ {2,}//g' $1 | sed -rb 's/\/\/.*$//g' | sed -rb 's/\/\*.*\*\///g' | sed -b '/^$/d'`

echo "$FILECONTENTS" | while read line; do
    case "$line" in
        '#'* ) printf '\n%s\n' "$line";;
        * ) printf '%s' "$line";;
    esac
done
