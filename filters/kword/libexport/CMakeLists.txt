

include_directories( ${KOFFICECORE_INCLUDES} )


########### next target ###############

set(kwordexportfilters_LIB_SRCS 
   TagProcessing.cc 
   KWEFStructures.cc 
   ProcessDocument.cc 
   KWEFUtil.cc 
   KWEFKWordLeader.cc 
   KWEFBaseWorker.cc )

kde4_automoc(${kwordexportfilters_LIB_SRCS})

kde4_add_library(kwordexportfilters SHARED ${kwordexportfilters_LIB_SRCS})

target_link_libraries(kwordexportfilters  ${KDE4_KDECORE_LIBS} kofficecore )

set_target_properties(kwordexportfilters PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kwordexportfilters  DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############






#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES = -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
#
#libkwordexportfilters_la_LDFLAGS = $(all_libraries) -version-info 1:1:0 -no-undefined
#
#libkwordexportfilters_la_LIBADD = $(KOFFICE_LIBS)
#
######## Files
#
#lib_LTLIBRARIES = libkwordexportfilters.la
#
#libkwordexportfilters_la_SOURCES = TagProcessing.cc KWEFStructures.cc\
#ProcessDocument.cc KWEFUtil.cc KWEFKWordLeader.cc KWEFBaseWorker.cc
#
#noinst_HEADERS = TagProcessing.h KWEFStructures.h\
#ProcessDocument.h KWEFUtil.h KWEFKWordLeader.h KWEFBaseWorker.h
#
#METASOURCES = AUTO
