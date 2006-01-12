
KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES = $(KOFFICE_INCLUDES) $(KOTEXT_INCLUDES)  \
           -I$(top_srcdir)/kspread -I$(top_srcdir)/kword $(all_includes)

kde_module_LTLIBRARIES = kwmailmerge_kspread.la

kwmailmerge_kspread_la_SOURCES = kwmailmerge_kspread.cpp kwmailmerge_kspread_config.cpp
kwmailmerge_kspread_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kwmailmerge_kspread_la_LIBADD = ../../libkwmailmerge_interface.la \
                                $(top_builddir)/kspread/libkspreadcommon.la $(LIB_KDEUI)

METASOURCES = AUTO

services_DATA = kwmailmerge_kspread.desktop
servicesdir = $(kde_servicesdir)
