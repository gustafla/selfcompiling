TARGET=demo
CAT_SRC=$(TARGET).cat.c
CC=gcc
MINIFY=cat
LFLAGS=-lSDL2 -lGL -lEGL
SHADERS=$(patsubst %.glsl,%.glsl.h,$(wildcard shaders/*.glsl))

demo: $(CAT_SRC) launcher.sh
	cp launcher.sh $(TARGET)
	$(MINIFY) $(CAT_SRC) | lzma -9e -T0 - >> $(TARGET)
	chmod +x $(TARGET)
	rm $(CAT_SRC)

debug: $(CAT_SRC)
	$(CC) -g $(LFLAGS) -o $(TARGET) $(CAT_SRC)
	rm $(CAT_SRC)

$(CAT_SRC): shaders.h api.h main.c
	cp api.h $(CAT_SRC)
	cat shaders.h >> $(CAT_SRC)
	cat main.c >> $(CAT_SRC)
	rm shaders.h

shaders.h: $(SHADERS)
	for f in shaders/*.h; do cat $$f >> shaders.h; done
	rm shaders/*.h

shaders/%.glsl.h:
	for f in shaders/*.glsl; do \
	F=`echo $$f | tr ./ _ | tr '[:lower:]' '[:upper:]'`; \
	echo "char const *$$F = \" \\" > "$$f.h"; \
	cat $$f | sed 's/$$/ \\/' >> "$$f.h"; \
	echo "\";" >> "$$f.h"; done
