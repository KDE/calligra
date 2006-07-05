include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOFFICEUI_INCLUDES} ${KOPAINTER_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon )

set(vpainter_SRCS
    vpainterfactory.cc
    vqpainter.cc
    )

kde4_automoc(${vpainter_SRCS})

kde4_add_library(vpainter ${vpainter_SRCS})

kde4_install_libtool_file( ${LIB_INSTALL_DIR} vpainter )

install(TARGETS vpainter  DESTINATION ${LIB_INSTALL_DIR})



########### next target ###############


########### install files ###############






#original Makefile.am contents follow:

#SUBDIRS = xrgbrender
#
#INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
#	-I$(srcdir)/.. \
#	-I$(srcdir)/xrgbrender \
#	-I$(srcdir)/../core \
#	$(LIBART_CFLAGS) \
#	$(all_includes)
#
#noinst_LTLIBRARIES = libvpainter.la
#
#noinst_HEADERS = \
#	vpainter.h \
#	vpainterfactory.h \
#	vqpainter.h \
#	art_render_pattern.h \
#	art_render_misc.h \
#	art_rgb_svp.h \
#	art_rgb.h \
#	art_rgba_affine.h \
#	vkopainter.h
#
#libvpainter_la_SOURCES = \
#	vpainterfactory.cc \
#	vqpainter.cc \
#	art_render_pattern.c \
#	art_render_misc.c \
#	art_rgb_svp.c \
#	art_rgb.c \
#	art_rgba_affine.c \
#	vkopainter.cc
#
#libvpainter_la_METASOURCES = \
#	AUTO
