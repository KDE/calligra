all:
	echo "make (unix|msvc)"

clean:
	rm -f a.out*

../src/libshttpd.a:
	cd ../src && $(MAKE) unix

unix: ../src/libshttpd.a
	$(CC) $(CFLAGS) example.c ../src/compat_unix.c \
		-I ../src  ../src/libshttpd.a

msvc:
	cl example.c /I..\src ..\src\shttpd.lib /MD
	rc dialog.rc
	cl example_win32_dialog_app.c dialog.res /I..\src \
		..\src\shttpd.lib user32.lib /MD
