include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOFFICEUI_INCLUDES} ${KOPAINTER_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon )

set(karbonshapes_SRCS
    vellipse.cc 
    vpolyline.cc 
    vpolygon.cc 
    vrectangle.cc 
    vsinus.cc 
    vspiral.cc 
    vstar.cc
    )

kde4_automoc(${karbonshapes_SRCS})

kde4_add_library(karbonshapes ${karbonshapes_SRCS})

kde4_install_libtool_file( ${LIB_INSTALL_DIR} karbonshapes )

install_targets(${LIB_INSTALL_DIR} karbonshapes )





########### next target ###############


########### install files ###############






#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
#	-I$(srcdir)/.. \
#	-I$(srcdir)/../core \
#	-I$(srcdir)/../commands \
#	$(all_includes)
#
#noinst_LTLIBRARIES = libkarbonshapes.la
#
#noinst_HEADERS = \
#	vellipse.h \
#	vpolyline.h \
#	vpolygon.h \
#	vrectangle.h \
#	vsinus.h \
#	vspiral.h \
#	vstar.h
#
#libkarbonshapes_la_SOURCES = \
#	vellipse.cc \
#	vpolyline.cc \
#	vpolygon.cc \
#	vrectangle.cc \
#	vsinus.cc \
#	vspiral.cc \
#	vstar.cc
#
#libkarbonshapes_la_METASOURCES = \
#	AUTO
#
