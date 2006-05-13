


include_directories(${CMAKE_SOURCE_DIR}/libs/kwmf )

########### next target ###############

set(mswriteimport_PART_SRCS 
   mswriteimport.cc 
   libmswrite_defs.cpp 
   structures_generated.cpp 
   structures_private.cpp 
   structures.cpp 
   libmswrite.cpp 
   ImportDialog.cc )

kde4_automoc(${mswriteimport_PART_SRCS})

kde4_add_ui3_files(mswriteimport_PART_SRCS ImportDialogUI.ui )

kde4_add_plugin(mswriteimport WITH_PREFIX ${mswriteimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} mswriteimport )

target_link_libraries(mswriteimport kofficecore)

install_targets(${PLUGIN_INSTALL_DIR} mswriteimport )


########### next target ###############

set(mswriteexport_PART_SRCS 
   mswriteexport.cc 
   libmswrite_defs.cpp 
   structures_generated.cpp 
   structures_private.cpp 
   structures.cpp 
   libmswrite.cpp )

kde4_automoc(${mswriteexport_PART_SRCS})

kde4_add_plugin(mswriteexport WITH_PREFIX ${mswriteexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} mswriteexport )

target_link_libraries(mswriteexport  ${KDE4_KDECORE_LIBS} kowmf kwordexportfilters )

install_targets(${PLUGIN_INSTALL_DIR} mswriteexport )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kword_mswrite_import.desktop kword_mswrite_export.desktop )





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) -I$(srcdir)/../libexport $(all_includes)
#
######## Files
#
#kde_module_LTLIBRARIES = libmswriteimport.la libmswriteexport.la
#
#libmswriteimport_la_SOURCES = mswriteimport.cc \
#								libmswrite_defs.cpp structures_generated.cpp structures_private.cpp structures.cpp libmswrite.cpp ImportDialogUI.ui ImportDialog.cc
#
#libmswriteimport_la_LDFLAGS = -module $(KDE_PLUGIN)
#libmswriteimport_la_LIBADD = $(KOFFICE_LIBS)
#
#libmswriteexport_la_SOURCES = mswriteexport.cc	\
#								libmswrite_defs.cpp structures_generated.cpp structures_private.cpp structures.cpp libmswrite.cpp
#libmswriteexport_la_LDFLAGS = -module $(KDE_PLUGIN)
#libmswriteexport_la_LIBADD = $(KOFFICE_LIBS) $(LIB_KOWMF) ../libexport/libkwordexportfilters.la
#
#noinst_HEADERS = mswriteimport.h mswriteexport.h \
#					  config.libmswrite.h \
#					  ImportDialog.h \
#					  ImportDialogUI.h \
#					  libmswrite_defs.h \
#					  libmswrite.h \
#					  list.h \
#					  structures_generated.h \
#					  structures.h \
#					  structures_private.h
#METASOURCES = AUTO
#
#service_DATA = kword_mswrite_import.desktop kword_mswrite_export.desktop
#servicedir = $(kde_servicesdir)
