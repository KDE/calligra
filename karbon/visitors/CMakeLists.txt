include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOPAINTER_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon )

set(karbonvisitors_SRCS
    vselectnodes.cc 
    vselectobjects.cc 
    vdrawselection.cc 
    vselectiondesc.cc 
    vtransformnodes.cc 
    )

kde4_automoc(${karbonvisitors_SRCS})

kde4_add_library(karbonvisitors ${karbonvisitors_SRCS})

kde4_install_libtool_file( ${LIB_INSTALL_DIR} karbonvisitors )

install_targets(${LIB_INSTALL_DIR} karbonvisitors )




########### next target ###############


########### install files ###############






#original Makefile.am contents follow:

#INCLUDES = \
#	$(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
#	-I$(srcdir)/.. \
#	-I$(srcdir)/../core \
#	-I$(srcdir)/../tools \
#	-I$(srcdir)/../render \
#	$(LIBFREETYPE_CFLAGS) $(all_includes)
#
#noinst_LTLIBRARIES = libkarbonvisitors.la
#
#noinst_HEADERS = \
#	vselectnodes.h \
#	vselectobjects.h \
#	vdrawselection.h \
#	vselectiondesc.h \
#	vtransformnodes.h 
#
#libkarbonvisitors_la_SOURCES = \
#	vselectnodes.cc \
#	vselectobjects.cc \
#	vdrawselection.cc \
#	vselectiondesc.cc \
#	vtransformnodes.cc 
#
#libkarbonvisitors_la_METASOURCES = \
#	AUTO
