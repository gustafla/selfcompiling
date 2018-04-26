demo: demo.c launcher.sh
	cp launcher.sh demo
	sh minify.sh demo.c | lzma -9e -T0 - >> demo
	chmod +x demo
