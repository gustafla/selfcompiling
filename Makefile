TARGET=demo
CAT_SRC=$(TARGET).out.c
CC=gcc
MINIFY=sh minify.sh
SHADER_MINIFY=TERM=xterm mono ~/misc/shader_minifier.exe --format none
LFLAGS=-lSDL2 -lGL -lEGL
SOURCES=api.c main.c
MINIFIED=$(SOURCES:.c=.c.min)
SHADERS=$(patsubst %.glsl, %.glsl.min.h.bin, $(wildcard shaders/*.glsl))

# Concatenate and compress demo sources into executable
$(TARGET): $(CAT_SRC) launcher.sh
	cp launcher.sh $(TARGET)
	cat $(CAT_SRC) | lzma -9e -T0 - >> $(TARGET)
	chmod +x $(TARGET)

.PHONY: debug clean

debug: $(CAT_SRC)
	$(CC) -g $(LFLAGS) -o $(TARGET) $(CAT_SRC)

clean:
	rm -f $(TARGET)
	find . -name "*.bin" -delete
	find . -name "*.min" -delete
	find . -name "*.out.c" -delete

# Concatenate sources to single source file in the right order
$(CAT_SRC): shaders.h.bin $(MINIFIED)
	cat shaders.h.bin > $(CAT_SRC)
	for f in $(MINIFIED); do cat $$f >> $(CAT_SRC); done

# Minify source files
%.c.min:
	$(MINIFY) $*.c > $@

# Concatenate minified shaders into one .h file
shaders.h.bin: $(SHADERS)
	echo > shaders.h.bin
	for f in shaders/*.h.bin; do cat $$f >> shaders.h.bin; done

# Minify and convert shaders to C source
shaders/%.glsl.min.h.bin:
	for f in shaders/*.glsl; do \
	$(SHADER_MINIFY) -o "$$f.min" "$$f"; done
	for f in shaders/*.glsl.min; do \
	F=`echo $$f | tr ./ _ | tr '[:lower:]' '[:upper:]'`; \
	echo "char *$$F=\"\\" > "$$f.h.bin"; \
	cat $$f | sed 's/$$/\\/' >> "$$f.h.bin"; \
	echo >> "$$f.h.bin"; \
	echo "\";" >> "$$f.h.bin"; done
