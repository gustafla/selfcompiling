#!/bin/sh

TERM=xterm # Needed to avoid Mono bugs
SOURCEFILES=`find * -name '*.frag' -o -name '*.vert'`
OUTPUTFILE="../shaders.h.out"

# Minify with Crtl-Alt-Test's tool when needed
if [ ! "$1" = "debug" ]; then
    for f in $SOURCEFILES; do
        mono ~/misc/shader_minifier.exe --format none -o "$f.min" "$f"
    done
fi

# Convert to C source
for f in $SOURCEFILES; do
    # Generate C identifiers/symbols for filenames
    F=`echo $f | tr ./ _ | tr '[:lower:]' '[:upper:]'`

    # Write C source code, only use minified when not debugging
    echo "char *$F=\"\\" > "$f.h.out"
    if [ "$1" = "debug" ]; then
        echo "shaders/$f\\" >> "$f.h.out" # only write filename
    else
        cat "$f.min" | sed 's/$/\\/' >> "$f.h.out" # useless use of cat?
        echo >> "$f.h.out"
    fi
    echo "\";" >> "$f.h.out"
done

# Concatenate all shaders to a file
echo > $OUTPUTFILE
for f in *.h.out; do
    cat $f >> $OUTPUTFILE
done
echo >> $OUTPUTFILE
