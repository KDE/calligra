include_directories( ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )

########### next target ###############

set(krosskexiapp_PART_SRCS kexiapppart.cpp kexiappmainwindow.cpp )

kde4_add_plugin(krosskexiapp ${krosskexiapp_PART_SRCS})

target_link_libraries(krosskexiapp  ${KDE4_KDECORE_LIBS} kexicore )

install(TARGETS krosskexiapp  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#INCLUDES = -I$(top_srcdir)/kexi $(KROSS_INCLUDES) $(all_includes)
#
#kde_module_LTLIBRARIES = krosskexiapp.la
#
#krosskexiapp_la_SOURCES = \
#    kexiapppart.cpp \
#    kexiappmainwindow.cpp \
#    kexiappmodule.cpp
#
#krosskexiapp_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
#krosskexiapp_la_LIBADD = \
#    $(LIB_QT) \
#    $(LIB_KDECORE) \
#    $(LIB_KROSS_API) \
#    $(LIB_KROSS_MAIN) \
#    $(top_builddir)/kexi/core/libkexicore.la
#
#METASOURCES = AUTO
#SUBDIRS = .
