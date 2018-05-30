#!/bin/sh

TERM=xterm

# Minify with Crtl-Alt-Test's tool
for f in `find * -name '*.frag' -o -name '*.vert'`; do
    mono ~/misc/shader_minifier.exe --format none -o "$f.min" "$f"
done

# Convert to C source
for f in *.min; do
    F=`echo $f | tr ./ _ | tr '[:lower:]' '[:upper:]'`
    echo "char *$F=\"\\" > "$f.h.out"
    cat $f | sed 's/$/\\/' >> "$f.h.out"
    echo >> "$f.h.out"
    echo "\";" >> "$f.h.out"
done

echo > ../shaders.h.out
for f in *.h.out; do
    cat $f >> ../shaders.h.out
done
echo >> ../shaders.h.out
