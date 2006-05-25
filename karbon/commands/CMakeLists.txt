include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOPAINTER_INCLUDES} ${CMAKE_SOURCE_DIR}/karbon )

set(karboncommands_SRCS
    valigncmd.cc 
    vbooleancmd.cc 
    vcleanupcmd.cc 
    vclipartcmd.cc 
    vclosepathcmd.cc 
    vcommand.cc 
    vdeletecmd.cc 
    vdistributecmd.cc 
    vfillcmd.cc 
    vflattencmd.cc 
    vgroupcmd.cc 
    vdeletenodescmd.cc 
    vlayercmd.cc 
    vreplacingcmd.cc 
    vshapecmd.cc 
    vstrokecmd.cc 
    vtransformcmd.cc 
    vinsertcmd.cc 
    vungroupcmd.cc 
    vzordercmd.cc
    )

kde4_automoc(${karboncommands_SRCS})

kde4_add_library(karboncommands ${karboncommands_SRCS})

kde4_install_libtool_file( ${LIB_INSTALL_DIR} karboncommands )

install_targets(${LIB_INSTALL_DIR} karboncommands )




########### next target ###############


########### install files ###############






#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) $(KOFFICECORE_INCLUDES) $(KOPAINTER_INCLUDES) \
#	-I$(srcdir)/.. \
#	-I$(srcdir)/../core \
#	-I$(srcdir)/../visitors \
#	$(all_includes)
#
#noinst_LTLIBRARIES = libkarboncommands.la
#
#noinst_HEADERS = \
#	valigncmd.h \
#	vbooleancmd.h \
#	vcleanupcmd.h \
#	vclipartcmd.h \
#	vclosepathcmd.h \
#	vcommand.h \
#	vdeletecmd.h \
#	vdistributecmd.h \
#	vfillcmd.h \
#	vflattencmd.h \
#	vgroupcmd.h \
#	vdeletenodescmd.h \
#	vlayercmd.h \
#	vreplacingcmd.h \
#	vshapecmd.h \
#	vstrokecmd.h \
#	vtransformcmd.h \
#	vinsertcmd.h \
#	vungroupcmd.h \
#	vzordercmd.h
#
#libkarboncommands_la_SOURCES = \
#	valigncmd.cc \
#	vbooleancmd.cc \
#	vcleanupcmd.cc \
#	vclipartcmd.cc \
#	vclosepathcmd.cc \
#	vcommand.cc \
#	vdeletecmd.cc \
#	vdistributecmd.cc \
#	vfillcmd.cc \
#	vflattencmd.cc \
#	vgroupcmd.cc \
#	vdeletenodescmd.cc \
#	vlayercmd.cc \
#	vreplacingcmd.cc \
#	vshapecmd.cc \
#	vstrokecmd.cc \
#	vtransformcmd.cc \
#	vinsertcmd.cc \
#	vungroupcmd.cc \
#	vzordercmd.cc
#
#libkarboncommands_la_METASOURCES = \
#	AUTO
#
