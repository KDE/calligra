include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOFFICEUI_INCLUDES} ${KOPAINTER_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon )

set(karbondockers_SRCS
    vcolordocker.cc
    vdocumentdocker.cc
    vstrokedocker.cc
    vstyledocker.cc
    vtransformdocker.cc
    )

kde4_automoc(${karbondockers_SRCS})

kde4_add_library(karbondockers ${karbondockers_SRCS})

kde4_install_libtool_file( ${LIB_INSTALL_DIR} karbondockers )

#target_link_libraries( karbondockers ${KDE4_CORE} karbonpart )

install(TARGETS karbondockers  DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############



########### next target ###############


########### install files ###############






#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
#	-I$(srcdir)/.. \
#	-I$(srcdir)/../core \
#	-I$(srcdir)/../commands \
#	-I$(srcdir)/../render \
#	-I$(srcdir)/../tools \
#	-I$(srcdir)/../widgets \
#	-I$(srcdir)/../dialogs \
#	$(all_includes)
#
#noinst_LTLIBRARIES = \
#	libkarbondockers.la
#
#noinst_HEADERS = \
#	vcolordocker.h \
#	vdocumentdocker.h \
#	vstrokedocker.h \
#	vstyledocker.h \
#	vtransformdocker.h
#
#libkarbondockers_la_SOURCES = \
#	dummy.cc \
#	vcolordocker.cc \
#	vdocumentdocker.cc \
#	vstrokedocker.cc \
#	vstyledocker.cc \
#	vtransformdocker.cc
#
#libkarbondockers_la_METASOURCES = AUTO
#
#DISTCLEANFILES = \
#	dummy.cc
#
#dummy.cc:
#	echo > dummy.cc
#
