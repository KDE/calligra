INCLUDES = -I$(top_srcdir) $(all_includes) $(KOFFICE_INCLUDES)

kde_module_LTLIBRARIES = kwmailmerge_kexi.la

kwmailmerge_kexi_la_SOURCES = kexikwmailmergebase.cpp kexikwmailmergebase.skel
kwmailmerge_kexi_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kwmailmerge_kexi_la_LIBADD = $(LIB_KDEUI) $(top_builddir)/kword/libkwmailmerge_interface.la -lDCOP

METASOURCES = AUTO

servicesdir=$(kde_servicesdir)
services_DATA = kwserialletter_kexi.desktop
