noinst_LTLIBRARIES =libtools.la

INCLUDES = -I$(top_srcdir)/kivio/kiviopart\
	-I$(top_srcdir)/kivio/kiviopart/kiviosdk\
	$(KOFFICE_INCLUDES) $(all_includes)
libtools_la_METASOURCES = AUTO
libtools_la_SOURCES = \
		tool.cpp\
		tool_controller.cpp
EXTRA_DIST = \
		tool.cpp\
		tool.h\
		tool_controller.cpp\
		tool_controller.h\
		kivio_tool.rc

rcdir = $(kde_datadir)/kivio
rc_DATA = kivio_tool.rc
