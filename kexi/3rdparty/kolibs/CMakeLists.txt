kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )


########### next target ###############


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#noinst_LTLIBRARIES = libkexikolibs.la
#
#libkexikolibs_la_SOURCES = \
#koGlobal.cc \
#koUnit.cc \
#koUnitWidgets.cc \
#koPageLayoutDia.cc \
#KoPageLayoutColumnsBase.ui \
#KoPageLayoutColumns.cpp \
#koPageLayout.cpp \
#KoPageLayoutHeaderBase.ui \
#KoPageLayoutHeader.cpp \
#KoPageLayoutSize.cpp
#
#noinst_HEADERS = 
#
#libkexikolibs_la_LDFLAGS = $(all_libraries) -Wnounresolved
#libkexikolibs_la_LIBADD =
#libkexikolibs_la_METASOURCES = AUTO
#
#SUBDIRS = .
#
## set the include path for X, qt and KDE
#
#INCLUDES= -I$(top_srcdir)/kexi $(all_includes)
#
#KDE_CXXFLAGS += -DSIMPLE_KOLIBS -DKOFFICECORE_EXPORT= -DKOFFICEUI_EXPORT=
