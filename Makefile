TARGET=demo
CAT_SRC=$(TARGET).out
CC=gcc
LFLAGS=$(shell sdl2-config --libs) -lGL -lGLEW -lm

SHADERS_TO_C=sh shaders_to_c.sh
debug:SHADERS_TO_C=sh shaders_to_c.sh debug

# shaders.h.out has to be excluded because trim can't deal with it
# music.xm.c has to be excluded because minify can't deal with it
HEADERS=api.c
SOURCES=xmplayer.c main.c

# Concatenate and compress demo sources into executable
$(TARGET): shaders.h.out $(CAT_SRC) launcher.sh
	cp shaders.h.out $(CAT_SRC).c
	sh trim.sh $(CAT_SRC) >> $(CAT_SRC).c
	cp launcher.sh $(TARGET)
	unifdef -x 2 -UDEBUG $(CAT_SRC).c | lzma -9e -T0 - >> $(TARGET)
	chmod +x $(TARGET)

.PHONY: debug clean

debug: shaders.h.out main.c
	$(CC) $(shell sdl2-config --cflags) -DDEBUG -O2 -g $(LFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET)
	find . -name "*.min" -delete
	find . -name "*.out" -delete
	find . -name "*.out.c" -delete

$(CAT_SRC): $(HEADERS) music.xm.c $(SOURCES)
	echo > $(CAT_SRC)
	for f in $(HEADERS); do cat $$f >> $(CAT_SRC); done
	cat music.xm.c >> $(CAT_SRC)
	echo > out.min
	for f in $(SOURCES); do cat $$f >> out.min; done
	minify $(patsubst %,-h %,$(HEADERS)) -h shaders.h.out out.min >> $(CAT_SRC)

shaders.h.out:
	cd shaders; $(SHADERS_TO_C)
