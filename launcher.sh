echo Decompressing...;a="${TMPDIR:-/tmp}"/i.c;sed 1d $0|lzcat>$a;echo Compiling...;cc -O2 -L/opt/vc/lib -lSDL2 -lGLESv2 -lm -o $a. $a;echo Done.;$a.;exit
