include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOFFICEUI_INCLUDES} ${KOPAINTER_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon ${CMAKE_SOURCE_DIR}/karbon/visitors ${FREETYPE_INCLUDE_DIR} ${FreeType2_INCLUDE_DIR} ${FONTCONFIG_INCLUDE_DIR} )
MESSAGE(STATUS "<FREETYPE_INCLUDE_DIR> <${FREETYPE_INCLUDE_DIR}>")

add_definitions(${FREETYPE_INCLUDE_DIR})

set(karboncore_SRCS
    vcolor.cc 
    vcomposite.cc 
    vcomposite_iface.cc  
    vdashpattern.cc 
    vdocument.cc 
    vfill.cc 
    vglobal.cc 
    vgradient.cc 
    vgroup.cc 
    vobject_iface.cc 
    vgroup_iface.cc 
    vimage.cc 
    vkarbonplugin.cc 
    vlayer.cc 
    vobject.cc 
    vpath.cc 
    vpattern.cc 
    vsegment.cc 
    vselection.cc 
    vstroke.cc 
    vvisitor.cc 
    vlayer_iface.cc 
    vtext.cc 
    vtext_iface.cc  
    vclipgroup.cc 
    vcursor.cc
    )

kde4_add_dcop_skels(karboncore_SRCS
    vcomposite_iface.h
    vobject_iface.h
    vgroup_iface.h
    vlayer_iface.h 
    vtext_iface.h
    )

kde4_automoc(${karboncore_SRCS})

kde4_add_library(karboncore ${karboncore_SRCS})

kde4_install_libtool_file( ${LIB_INSTALL_DIR} karboncore )

target_link_libraries(karboncore ${FREETYPE_LIBRARIES} ${FONTCONFIG_LIBRARIES})

install_targets(${LIB_INSTALL_DIR} karboncore )




########### next target ###############


########### install files ###############






#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
#	-I$(srcdir)/.. \
#	-I$(srcdir)/../render \
#	-I$(srcdir)/../visitors \
#	$(LIBFREETYPE_CFLAGS) $(LIBFONTCONFIG_CFLAGS) $(all_includes)
#
#noinst_LTLIBRARIES = libkarboncore.la
#
#noinst_HEADERS = \
#	vcolor.h \
#	vcomposite.h \
#	vcomposite_iface.h \
#	vdashpattern.h \
#	vdocument.h \
#	vfill.h \
#	vglobal.h \
#	vgradient.h \
#	vgroup.h \
#	vobject_iface.h \
#	vgroup_iface.h \
#	vimage.h \
#	vkarbonplugin.h \
#	vlayer.h \
#	vobject.h \
#	vpath.h \
#	vpattern.h \
#	vsegment.h \
#	vselection.h \
#	vstroke.h \
#	vvisitor.h \
#	vlayer_iface.h \
#	vtext.h \
#	vtext_iface.h \
#	vclipgroup.h \
#	vcursor.h
#
#libkarboncore_la_SOURCES = \
#	vcolor.cc \
#	vcomposite.cc \
#	vcomposite_iface.cc \
#	vcomposite_iface.skel \
#	vdashpattern.cc \
#	vdocument.cc \
#	vfill.cc \
#	vglobal.cc \
#	vgradient.cc \
#	vgroup.cc \
#	vobject_iface.cc \
#	vobject_iface.skel \
#	vgroup_iface.cc \
#	vgroup_iface.skel \
#	vimage.cc \
#	vkarbonplugin.cc \
#	vlayer.cc \
#	vobject.cc \
#	vpath.cc \
#	vpattern.cc \
#	vsegment.cc \
#	vselection.cc \
#	vstroke.cc \
#	vvisitor.cc \
#	vlayer_iface.cc \
#	vlayer_iface.skel \
#	vtext.cc \
#	vtext_iface.cc \
#	vtext_iface.skel \
#	vclipgroup.cc \
#	vcursor.cc
#
#libkarboncore_la_LIBADD = $(LIBFONTCONFIG_LIBS)
#
#libkarboncore_la_METASOURCES = \
#	AUTO
