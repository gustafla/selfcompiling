echo Decompressing...;a="${TMPDIR:-/tmp}"/i.c;sed 1d $0|lzcat>$a;echo Compiling...;cc -O2 -lSDL2 -lGL -lm -o $a. $a;echo Done.;$a.;exit
