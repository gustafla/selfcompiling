TARGET=demo

# Config for debug builds, for release build flags edit launcher scripts
CC=gcc
PKGS=sdl2 glesv2
CFLAGS=$(shell pkg-config --cflags $(PKGS)) -O2 -DDEBUG -g
LDLIBS=$(shell pkg-config --libs $(PKGS)) -lm

# shaders.h.out has to be excluded because trim can't deal with it
# music.xm.c has to be excluded because minify can't deal with it
SOURCES=xmplayer.c main.c
CAT_SRC=$(TARGET).out

LAUNCHER=launcher.sh

SHADERS_TO_C=sh shaders_to_c.sh
debug:SHADERS_TO_C=sh shaders_to_c.sh debug

.PHONY: debug clean brcm

# Concatenate and compress demo sources into executable
$(TARGET): shaders.h.out $(CAT_SRC) $(LAUNCHER)
	cp shaders.h.out $(CAT_SRC).c
	sh trim.sh $(CAT_SRC) >> $(CAT_SRC).c
	sh trim.sh $(LAUNCHER) > $(TARGET)
	echo >> $(TARGET)
	unifdef -x 2 -UDEBUG $(CAT_SRC).c | lzma -9e -T0 - >> $(TARGET)
	chmod +x $(TARGET)

brcm:
	+make LAUNCHER=brcm_launcher.sh

debug: shaders.h.out main.c
	$(CC) main.c -o $(TARGET) $(CFLAGS) $(LDLIBS)

clean:
	rm -f $(TARGET)
	find . -name "*.min" -delete
	find . -name "*.out" -delete
	find . -name "*.out.c" -delete

$(CAT_SRC): $(HEADERS) music.xm.c $(SOURCES)
	cp music.xm.c $(CAT_SRC)
	echo > out.min
	for f in $(SOURCES); do cat $$f >> out.min; done
	minify -h api.h -h shaders.h.out out.min >> $(CAT_SRC)

shaders.h.out:
	cd shaders; $(SHADERS_TO_C)
