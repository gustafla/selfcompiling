#!/bin/sh
LOG=""

while true; do
    LOG=`make -B debug`
    ret=$?
    if [ $ret -ne 0 ]; then echo $LOG | less; fi
    LOG=`./demo`
    echo $LOG | less
done

