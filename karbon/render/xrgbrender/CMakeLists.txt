kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = $(all_includes)
#
#noinst_LTLIBRARIES = libkarbonxrgbrender.la
#
#libkarbonxrgbrender_la_SOURCES = gdk-pixbuf-xlib.c gdk-pixbuf-xlib-render.c gdk-pixbuf-xlib-drawable.c gdk-pixbuf-xlibrgb.c
