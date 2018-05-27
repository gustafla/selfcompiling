#!/bin/sh
LOG=""

while true; do
    LOG=`make -B debug 2>&1 >/dev/null`
    ret=$?
    if [ $ret -ne 0 ]; then
        echo $LOG | less
    else
        LOG=`./demo`
        echo $LOG | less
    fi
done

