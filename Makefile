demo: demo.c launcher.sh
	cp launcher.sh demo
	sh minify.sh demo.c | lzma -9e -T0 - >> demo
	chmod +x demo
	rm demo.c

demo.c:
	cp sdl_api.h demo.c
	cat main.c >> demo.c
