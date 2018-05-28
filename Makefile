TARGET=demo
CAT_SRC=$(TARGET).out
CC=gcc
MINIFY=sh trim.sh
SHADER_MINIFY=TERM=xterm mono ~/misc/shader_minifier.exe --format none
LFLAGS=$(shell sdl2-config --libs) -lGL -lGLEW -lm

HEADERS=api.c
SOURCES=unreeeal_superhero_3.xm.c
SOURCES_TO_MINIFY=xmplayer.c main.c

SHADERS=$(patsubst %.glsl, %.glsl.min.h.out, $(wildcard shaders/*.glsl))

# Concatenate and compress demo sources into executable
$(TARGET): shaders.h.out $(CAT_SRC) launcher.sh
	cp shaders.h.out $(CAT_SRC).c
	$(MINIFY) $(CAT_SRC) >> $(CAT_SRC).c
	cp launcher.sh $(TARGET)
	unifdef -x 2 -UDEBUG $(CAT_SRC).c | lzma -9e -T0 - >> $(TARGET)
	chmod +x $(TARGET)

.PHONY: debug clean

debug: shaders.h.out main.c
	unifdef -x 2 -DDEBUG -o $(CAT_SRC).c main.c
	$(CC) $(shell sdl2-config --cflags) -O2 -g $(LFLAGS) -o $(TARGET) $(CAT_SRC).c

clean:
	rm -f $(TARGET)
	find . -name "*.min" -delete
	find . -name "*.out" -delete
	find . -name "*.out.c" -delete

# Concatenate sources to single source file in the right order
$(CAT_SRC): shaders.h.out $(HEADERS) $(SOURCES) $(SOURCES_TO_MINIFY)
	echo > $(CAT_SRC)
	for f in $(HEADERS); do cat $$f >> $(CAT_SRC); done
	for f in $(SOURCES); do cat $$f >> $(CAT_SRC); done
	echo > out.min
	for f in $(SOURCES_TO_MINIFY); do cat $$f >> out.min; done
	minify -h $(HEADERS) -h shaders.h.out out.min >> $(CAT_SRC)

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
