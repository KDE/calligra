

include_directories(
	${CMAKE_SOURCE_DIR}/kchart 
	${CMAKE_SOURCE_DIR}/kchart/kdchart 
	${CMAKE_SOURCE_DIR}/interfaces 
	${CMAKE_SOURCE_DIR}/filters/libdialogfilter 
	${KDE4_INCLUDE_DIR} 
	${QT_INCLUDES}  )


########### next target ###############

set(kchartimageexport_LIB_SRCS imageexport.cpp ${libdialogfilter_SRCS})

kde4_automoc(${kchartimageexport_LIB_SRCS})

kde4_add_library(kchartimageexport SHARED ${kchartimageexport_LIB_SRCS})

target_link_libraries(kchartimageexport  ${KDE4_KDECORE_LIBS} kchartcommon)

set_target_properties(kchartimageexport PROPERTIES VERSION 4.0.0 SOVERSION 4 )
install_targets(${LIB_INSTALL_DIR} kchartimageexport )


########### install files ###############






#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) \
#        -I$(top_srcdir)/kchart \
#        -I$(top_srcdir)/kchart/kdchart \
#        -I$(top_srcdir)/interfaces \
#        -I$(top_srcdir)/filters/libdialogfilter \
#        $(all_includes)
#
######## Files
#
#lib_LTLIBRARIES = libkchartimageexport.la
#
#libkchartimageexport_la_SOURCES = imageexport.cpp
#libkchartimageexport_la_LDFLAGS = $(all_libraries) -version-info 4:0 -no-undefined
#libkchartimageexport_la_LIBADD = ../../../kchart/libkchartcommon.la ../../../filters/libdialogfilter/libdialogfilter.la $(KOFFICE_LIBS)
#
#noinst_HEADERS = \
#	imageexport.h
#
#METASOURCES = AUTO
#
#
