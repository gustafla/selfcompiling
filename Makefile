TARGET=demo
CAT_SRC=$(TARGET).out
CC=gcc
MINIFY=sh minify.sh
SHADER_MINIFY=TERM=xterm mono ~/misc/shader_minifier.exe --format none
LFLAGS=-lSDL2 -lGL -lm
SOURCES=api.c xmplayer.c main.c
SHADERS=$(patsubst %.glsl, %.glsl.min.h.out, $(wildcard shaders/*.glsl))

# Concatenate and compress demo sources into executable
$(TARGET): shaders.h.out $(CAT_SRC) launcher.sh
	cp shaders.h.out $(CAT_SRC).c
	$(MINIFY) $(CAT_SRC) >> $(CAT_SRC).c
	cp launcher.sh $(TARGET)
	cat $(CAT_SRC).c | lzma -9e -T0 - >> $(TARGET)
	chmod +x $(TARGET)

.PHONY: debug clean

debug: shaders.h.out $(CAT_SRC)
	cp shaders.h.out $(CAT_SRC).c
	cat $(CAT_SRC) >> $(CAT_SRC).c
	$(CC) -g $(LFLAGS) -o $(TARGET) $(CAT_SRC).c

clean:
	rm -f $(TARGET)
	find . -name "*.min" -delete
	find . -name "*.out" -delete
	find . -name "*.out.c" -delete

# Concatenate sources to single source file in the right order
$(CAT_SRC): $(SOURCES)
	echo > $(CAT_SRC)
	for f in $(SOURCES); do cat $$f >> $(CAT_SRC); done

# Concatenate minified shaders into one .h file
shaders.h.out: $(SHADERS)
	echo > shaders.h.out
	for f in shaders/*.h.out; do cat $$f >> shaders.h.out; done

# Minify and convert shaders to C source
shaders/%.glsl.min.h.out:
	for f in shaders/*.glsl; do \
	$(SHADER_MINIFY) -o "$$f.min" "$$f"; done
	for f in shaders/*.glsl.min; do \
	F=`echo $$f | tr ./ _ | tr '[:lower:]' '[:upper:]'`; \
	echo "char *$$F=\"\\" > "$$f.h.out"; \
	cat $$f | sed 's/$$/\\/' >> "$$f.h.out"; \
	echo >> "$$f.h.out"; \
	echo "\";" >> "$$f.h.out"; done
