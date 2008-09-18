

include_directories( ${CMAKE_SOURCE_DIR}/kexi/formeditor ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/scripting ${CMAKE_SOURCE_DIR}/kexi/core ${KDE4_INCLUDES}  )


########### next target ###############


########### install files ###############






#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#noinst_LTLIBRARIES = libkformdesigner_scripting.la
#
#libkformdesigner_scripting_la_SOURCES = formscript.cpp scriptIO.cpp scriptmanager.cpp
#
#libkformdesigner_scripting_la_LDFLAGS = $(all_libraries) -Wnounresolved
#libkformdesigner_scripting_la_LIBADD = $(top_builddir)/kexi/formeditor/libkformdesigner.la
#
#libkformdesigner_scripting_la_METASOURCES = AUTO
#
#SUBDIRS = .
#
#INCLUDES= -I$(top_srcdir)/kexi/formeditor \
#	-I$(top_srcdir)/kexi \
#	-I$(top_srcdir)/kexi/scripting \
#	-I$(top_srcdir)/kexi/core $(all_includes)
#
