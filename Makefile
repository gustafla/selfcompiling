TARGET=demo
CAT_SRC=$(TARGET).out
CC=gcc
LFLAGS=$(shell sdl2-config --libs) -lGL -lGLEW -lm

HEADERS=api.c
SOURCES=unreeeal_superhero_3.xm.c
SOURCES_TO_MINIFY=xmplayer.c main.c

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

$(CAT_SRC): shaders.h.out $(HEADERS) $(SOURCES) $(SOURCES_TO_MINIFY)
	echo > $(CAT_SRC)
	for f in $(HEADERS); do cat $$f >> $(CAT_SRC); done
	for f in $(SOURCES); do cat $$f >> $(CAT_SRC); done
	echo > out.min
	for f in $(SOURCES_TO_MINIFY); do cat $$f >> out.min; done
	minify -h $(HEADERS) -h shaders.h.out out.min >> $(CAT_SRC)

shaders.h.out:
	cd shaders; ./shaders_to_c.sh
