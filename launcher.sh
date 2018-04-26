a=/tmp/i.c;sed 1d $0|lzcat>$a;cc -lSDL2 -o $a. $a;$a.;exit
