INCLUDES = $(all_includes) $(KOFFICE_INCLUDES)

kde_module_LTLIBRARIES = kwmailmerge_kexi.la

kwmailmerge_kexi_la_SOURCES = kexikwmailmergebase.cpp kexikwmailmergebase.skel
kwmailmerge_kexi_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
kwmailmerge_kexi_la_LIBADD = $(LIB_KDEUI) -lkwmailmerge_interface -lDCOP

METASOURCES = AUTO

servicedir=$(kde_servicedir)
services_DATA = kwserialletter_kexi.desktop
