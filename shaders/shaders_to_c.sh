#!/bin/sh

TERM=xterm
SOURCEFILES=`find * -name '*.frag' -o -name '*.vert'`

# Minify with Crtl-Alt-Test's tool
for f in $SOURCEFILES; do
    mono ~/misc/shader_minifier.exe --format none -o "$f.min" "$f"
done

# Convert to C source
for f in *.min; do
    f_clean=`basename $f .min`
    F=`echo $f_clean | tr ./ _ | tr '[:lower:]' '[:upper:]'`
    echo "char *$F=\"\\" > "$f.h.out"
    if [ "$1" = "debug" ]; then
        echo "shaders/$f_clean" >> "$f.h.out"
    else
        cat $f | sed 's/$/\\/' >> "$f.h.out"
        echo >> "$f.h.out"
    fi
    echo "\";" >> "$f.h.out"
done

echo > ../shaders.h.out
for f in *.h.out; do
    cat $f >> ../shaders.h.out
done
echo >> ../shaders.h.out
