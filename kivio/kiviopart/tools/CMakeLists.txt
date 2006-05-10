kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kivio/kiviopart ${CMAKE_SOURCE_DIR}/kivio/kiviopart/kiviosdk ${KDE4_INCLUDES}  )


########### next target ###############


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio FILES  kivio_tool.rc )

kde4_footer()



#original Makefile.am contents follow:

#noinst_LTLIBRARIES =libtools.la
#
#INCLUDES = -I$(top_srcdir)/kivio/kiviopart\
#	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
#	$(KOFFICE_INCLUDES) $(all_includes)
#libtools_la_METASOURCES = AUTO
#libtools_la_SOURCES = kivio_plugin.cpp kivio_pluginmanager.cpp \
#	kivio_mousetool.cpp mousetoolaction.cpp
#EXTRA_DIST = \
#	kivio_tool.rc
#
#rcdir = $(kde_datadir)/kivio
#rc_DATA = kivio_tool.rc
#noinst_HEADERS = kivio_plugin.h kivio_pluginmanager.h kivio_mousetool.h \
#	mousetoolaction.h
