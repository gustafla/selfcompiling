a=/tmp/i.c;sed 1d $0|lzcat>$a;cc -O2 -lSDL2 -lGL -lEGL -o $a. $a;$a.;exit
