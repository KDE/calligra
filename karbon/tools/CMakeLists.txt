kde4_header()




########### next target ###############

set(karbon_defaulttools_PART_SRCS 
   vcurvefit.cc 
   vrotatetool.cc 
   vselectnodestool.cc 
   vselecttool.cc 
   vsheartool.cc 
   vellipsetool.cc 
   vgradienttool.cc 
   vpatterntool.cc 
   vpenciltool.cc 
   vpolygontool.cc 
   vpolylinetool.cc 
   vrectangletool.cc 
   vroundrecttool.cc 
   vshapetool.cc 
   vsinustool.cc 
   vspiraltool.cc 
   vstartool.cc 
   vtexttool.cc 
   vdefaulttools.cc )

kde4_automoc(${karbon_defaulttools_PART_SRCS})

kde4_add_plugin(karbon_defaulttools ${karbon_defaulttools_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} karbon_defaulttools )

target_link_libraries(karbon_defaulttools  ${KDE4_KDECORE_LIBS} ${KDE4_KPARTS_LIBS} kofficeui karboncommon )

install_targets(${PLUGIN_INSTALL_DIR} karbon_defaulttools )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  karbondefaulttools.desktop )

kde4_footer()



#original Makefile.am contents follow:

#kde_services_DATA = karbondefaulttools.desktop
#
#INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
#	-I$(srcdir)/.. \
#	-I$(srcdir)/../commands \
#	-I$(srcdir)/../core \
#	-I$(srcdir)/../dialogs \
#	-I$(srcdir)/../widgets \
#	-I$(srcdir)/../dockers \
#	-I$(srcdir)/../render \
#	-I$(srcdir)/../shapes \
#	-I$(srcdir)/../visitors \
#	$(all_includes)
#
#kde_module_LTLIBRARIES = karbon_defaulttools.la
#
#noinst_HEADERS = \
#	vcurvefit.h \
#	vrotatetool.h \
#	vselectnodestool.h \
#	vselecttool.h \
#	vsheartool.h \
#	vellipsetool.h \
#	vgradienttool.h \
#	vpatterntool.h \
#	vpenciltool.h \
#	vpolygontool.h \
#	vpolylinetool.h \
#	vrectangletool.h \
#	vroundrecttool.h \
#	vshapetool.h \
#	vsinustool.h \
#	vspiraltool.h \
#	vstartool.h \
#	vtexttool.h \
#	vdefaulttools.h
#
#karbon_defaulttools_la_SOURCES = \
#	vcurvefit.cc \
#	vrotatetool.cc \
#	vselectnodestool.cc \
#	vselecttool.cc \
#	vsheartool.cc \
#	vellipsetool.cc \
#	vgradienttool.cc \
#	vpatterntool.cc \
#	vpenciltool.cc \
#	vpolygontool.cc \
#	vpolylinetool.cc \
#	vrectangletool.cc \
#	vroundrecttool.cc \
#	vshapetool.cc \
#	vsinustool.cc \
#	vspiraltool.cc \
#	vstartool.cc \
#	vtexttool.cc \
#	vdefaulttools.cc
#
#karbon_defaulttools_la_LIBADD = $(LIB_KPARTS) $(LIB_KOFFICEUI) ../libkarboncommon.la
#karbon_defaulttools_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#
#karbon_defaulttools_la_METASOURCES = \
#	AUTO
