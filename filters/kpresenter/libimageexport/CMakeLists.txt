

include_directories( ${CMAKE_SOURCE_DIR}/kpresenter ${CMAKE_SOURCE_DIR}/lib/kotext ${KDE4_INCLUDES}  )


########### next target ###############

set(kpresenterimageexport_LIB_SRCS imageexport.cpp )

kde4_automoc(${kpresenterimageexport_LIB_SRCS})

kde4_add_library(kpresenterimageexport SHARED ${kpresenterimageexport_LIB_SRCS})

target_link_libraries(kpresenterimageexport  ${KDE4_KDECORE_LIBS} kpresenterprivate )

set_target_properties(kpresenterimageexport PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kpresenterimageexport  DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############






#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) \
#	-I$(top_srcdir)/kpresenter \
#        -I$(top_srcdir)/lib/kotext \
#	$(all_includes)
#
######## Files
#
#lib_LTLIBRARIES = libkpresenterimageexport.la
#
#libkpresenterimageexport_la_SOURCES = imageexport.cpp
#libkpresenterimageexport_la_LDFLAGS = $(all_libraries) -version-info 4:0 -no-undefined
#libkpresenterimageexport_la_LIBADD = ../../../kpresenter/libkpresenterprivate.la $(KOFFICE_LIBS)
#noinst_HEADERS = \
#	imageexport.h
#
#METASOURCES = AUTO
#
#
