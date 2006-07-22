fontdir = $(prefix)/share/apps/kformula/fonts
font_DATA = cmex10.ttf Arev.ttf ArevIt.ttf ArevBd.ttf ArevBI.ttf

EXTRA_DIST = $(font_DATA) README LICENSE.BAKOMA LICENSE.BITSTREAM LICENCE.AREV

# This uses -hook instead of -local so that it is run after fonts are installed
install-data-hook:
		-mkfontscale $(DESTDIR)$(fontdir)
		-mkfontdir $(DESTDIR)$(fontdir)


