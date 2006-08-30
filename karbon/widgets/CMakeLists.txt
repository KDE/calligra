include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOFFICEUI_INCLUDES} ${KOPAINTER_INCLUDES} ${FLAKE_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon )

set(karbonwidgets_SRCS
    vcanvas.cc
    vcolorslider.cc
    vgradientwidget.cc
    vgradienttabwidget.cc
    vreference.cc
    vstrokefillpreview.cc
    vtranslate.cc
    vselecttoolbar.cc
    vsmallpreview.cc
    vstatebutton.cc
    vtypebuttonbox.cc
    vruler.cc
    )

kde4_automoc(${karbonwidgets_SRCS})

kde4_add_library(karbonwidgets ${karbonwidgets_SRCS})

kde4_install_libtool_file( ${LIB_INSTALL_DIR} karbonwidgets )

target_link_libraries(karbonwidgets flake) 

install(TARGETS karbonwidgets  DESTINATION ${LIB_INSTALL_DIR})



########### next target ###############


########### install files ###############






#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
#	-I$(srcdir)/.. \
#	-I$(srcdir)/../core \
#	-I$(srcdir)/../commands \
#	-I$(srcdir)/../render \
#	-I$(srcdir)/../dialogs \
#	-I$(srcdir)/../dockers \
#	-I$(srcdir)/../tools \
#	$(all_includes)
#
#noinst_LTLIBRARIES = \
#	libkarbonwidgets.la
#
#noinst_HEADERS = \
#	vcanvas.h \
#	vcolorslider.h \
#	vgradientwidget.h \
#	vgradienttabwidget.h \
#	vreference.h \
#	vstrokefillpreview.h \
#	vtranslate.h \
#	vselecttoolbar.h \
#	vsmallpreview.h \
#	vstatebutton.h \
#	vtoolbox.h \
#	vtypebuttonbox.h \
#	vruler.h
#
#libkarbonwidgets_la_SOURCES = \
#	dummy.cc \
#	vcanvas.cc \
#	vcolorslider.cc \
#	vgradientwidget.cc \
#	vgradienttabwidget.cc \
#	vreference.cc \
#	vstrokefillpreview.cc \
#	vtranslate.cc \
#	vselecttoolbar.cc \
#	vsmallpreview.cc \
#	vstatebutton.cc \
#	vtoolbox.cc \
#	vtypebuttonbox.cc \
#	vruler.cc
#
#libkarbonwidgets_la_METASOURCES = \
#	AUTO
#
#DISTCLEANFILES = \
#	dummy.cc
#
#dummy.cc:
#	echo > dummy.cc
#
