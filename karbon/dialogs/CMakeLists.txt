include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOPAINTER_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon )

set(karbondialogs_SRCS
    vcolordlg.cc 
    vconfiguredlg.cc 
    vcolortab.cc 
    )

kde4_automoc(${karbondialogs_SRCS})

kde4_add_library(karbondialogs ${karbondialogs_SRCS})

kde4_install_libtool_file( ${LIB_INSTALL_DIR} karbondialogs )

install(TARGETS karbondialogs  DESTINATION ${LIB_INSTALL_DIR})




########### next target ###############


########### install files ###############






#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
#	-I$(srcdir)/.. \
#	-I$(srcdir)/../core \
#	-I$(srcdir)/../commands \
#	$(all_includes)
#
#noinst_LTLIBRARIES = \
#	libkarbondialogs.la
#
#noinst_HEADERS = \
#	vcolordlg.h \
#	vconfiguredlg.h \
#	vcolortab.h
#
#libkarbondialogs_la_SOURCES = \
#	vcolordlg.cc \
#	vconfiguredlg.cc \
#	vcolortab.cc
#
#libkarbondialogs_la_METASOURCES = \
#	AUTO
#
