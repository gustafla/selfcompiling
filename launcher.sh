echo Decompressing...;a="${TMPDIR:-/tmp}"/i.c;sed 1d $0|lzcat>$a;echo Compiling...;cc -O2 `pkg-config --cflags --libs sdl2 glesv2` -lm -o $a. $a;echo Done.;$a.;exit
