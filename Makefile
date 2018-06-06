TARGET=demo
CAT_SRC=$(TARGET).out

CC=gcc
CFLAGS=$(shell pkg-config --cflags sdl2) -O2 -DDEBUG -g
LFLAGS=$(shell pkg-config --libs sdl2) -lGLESv2 -lm

SHADERS_TO_C=sh shaders_to_c.sh
debug:SHADERS_TO_C=sh shaders_to_c.sh debug

# shaders.h.out has to be excluded because trim can't deal with it
# music.xm.c has to be excluded because minify can't deal with it
SOURCES=xmplayer.c main.c

# Concatenate and compress demo sources into executable
$(TARGET): shaders.h.out $(CAT_SRC) launcher.sh
	cp shaders.h.out $(CAT_SRC).c
	sh trim.sh $(CAT_SRC) >> $(CAT_SRC).c
	sh trim.sh launcher.sh > $(TARGET)
	echo >> $(TARGET)
	unifdef -x 2 -UDEBUG $(CAT_SRC).c | lzma -9e -T0 - >> $(TARGET)
	chmod +x $(TARGET)

.PHONY: debug clean

debug: shaders.h.out main.c
	$(CC) $(CFLAGS) $(LFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET)
	find . -name "*.min" -delete
	find . -name "*.out" -delete
	find . -name "*.out.c" -delete

$(CAT_SRC): $(HEADERS) music.xm.c $(SOURCES)
	cp music.xm.c $(CAT_SRC)
	echo > out.min
	for f in $(SOURCES); do cat $$f >> out.min; done
	minify -h api.c -h shaders.h.out out.min >> $(CAT_SRC)

shaders.h.out:
	cd shaders; $(SHADERS_TO_C)
