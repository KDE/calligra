

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/main/printing ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/libs/ ${CMAKE_SOURCE_DIR}/libs/main ${KDE4_INCLUDES}  )


########### next target ###############


########### install files ###############






#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#noinst_LTLIBRARIES = libkeximainprinting.la
#
#libkeximainprinting_la_SOURCES = \
#kexisimpleprintingengine.cpp \
#kexisimpleprintingpart.cpp \
#kexisimpleprintingpagesetupbase.ui \
#kexisimpleprintingpagesetup.cpp \
#kexisimpleprintpreviewwindow.cpp
#
#noinst_HEADERS = kexisimpleprintpreviewwindow_p.h
#
#libkeximainprinting_la_LDFLAGS = $(all_libraries) -Wnounresolved
#libkeximainprinting_la_LIBADD = \
#	$(top_builddir)/lib/komain/libkomain.la \
#	$(top_builddir)/lib//lib.la \
#	../../widget/libkexiextendedwidgets.la
#
#libkeximainprinting_la_METASOURCES = AUTO
#
#SUBDIRS = .
#
## set the include path for X, qt and KDE
#
#INCLUDES= -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/main/printing \
#	-I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi/widget \
#	-I$(top_builddir)/lib/ -I$(top_srcdir)/lib/ \
#	-I$(top_srcdir)/lib/komain \
#	-I$(top_builddir)/kexi/widget $(all_includes)
#
##KDE_CXXFLAGS += -DSIMPLE_KOLIBS -DKOFFICECORE_EXPORT= -DKOGUIUTILS_EXPORT=
