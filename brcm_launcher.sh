echo Decompressing...;
a="${TMPDIR:-/tmp}"/i.c;
sed 1d $0|lzcat>$a;
echo Compiling...;
cc $a -o $a. -O2 `PKG_CONFIG_PATH=/opt/vc/lib/pkgconfig pkg-config --cflags --libs sdl2 brcmglesv2` -lm;
echo Done.;
SDL_VIDEODRIVER=rpi $a.;
exit
