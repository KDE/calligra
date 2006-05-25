include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOFFICEUI_INCLUDES} ${KOPAINTER_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon ${LIBART_INCLUDE_DIR} )

set(karbonxrgbrender_SRCS
    gdk-pixbuf-xlib.c 
    gdk-pixbuf-xlib-render.c 
    gdk-pixbuf-xlib-drawable.c 
    gdk-pixbuf-xlibrgb.c
    )

kde4_automoc(${karbonxrgbrender_SRCS})

kde4_add_library(karbonxrgbrender ${karbonxrgbrender_SRCS})

kde4_install_libtool_file( ${LIB_INSTALL_DIR} karbonxrgbrender )

target_link_libraries(karbonxrgbrender ${LIBART_LIBS} )

install_targets(${LIB_INSTALL_DIR} karbonxrgbrender )





########### next target ###############


########### install files ###############






#original Makefile.am contents follow:

#INCLUDES = $(all_includes)
#
#noinst_LTLIBRARIES = libkarbonxrgbrender.la
#
#libkarbonxrgbrender_la_SOURCES = gdk-pixbuf-xlib.c gdk-pixbuf-xlib-render.c gdk-pixbuf-xlib-drawable.c gdk-pixbuf-xlibrgb.c
