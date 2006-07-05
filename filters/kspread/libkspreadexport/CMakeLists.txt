

include_directories( ${CMAKE_SOURCE_DIR}/kspread ${KDE4_INCLUDES}  )


########### next target ###############

set(kspreadexport_LIB_SRCS KSpreadLeader.cc KSpreadBaseWorker.cc )

kde4_automoc(${kspreadexport_LIB_SRCS})

kde4_add_library(kspreadexport SHARED ${kspreadexport_LIB_SRCS})

target_link_libraries(kspreadexport  ${KDE4_KDECORE_LIBS} )

set_target_properties(kspreadexport PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kspreadexport  DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############






#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) -I$(top_srcdir)/kspread -I$(srcdir)/../libkspreadexport $(KOFFICE_INCLUDES) \
#	          $(KOTEXT_INCLUDES) $(all_includes)
#
#libkspreadexport_la_LDFLAGS = $(all_libraries) -module -version-info 1:0:0 -no-undefined
#
#libkspreadexport_la_LIBADD = $(KOFFICE_LIBS) ../../../kspread/libkspreadcommon.la
#
######## Files
#
#lib_LTLIBRARIES = libkspreadexport.la
#
#libkspreadexport_la_SOURCES = KSpreadLeader.cc KSpreadBaseWorker.cc
#
#noinst_HEADERS = KSpreadLeader.h KSpreadBaseWorker.h
#
#METASOURCES = AUTO
