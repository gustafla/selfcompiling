PKG_CONFIG_PATH=/opt/vc/lib/pkgconfig;
SDL_VIDEODRIVER=rpi;
echo Decompressing...;
a="${TMPDIR:-/tmp}"/i.c;
sed 1d $0|lzcat>$a;
echo Compiling...;
cc $a -o $a. -O2 `pkg-config --cflags --libs sdl2 brcmglesv2` -lm;
echo Done.;
$a.;
exit
