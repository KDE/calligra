
kde_module_LTLIBRARIES = kwmailmerge_kexi.la

kwmailmerge_kexi_la_SOURCES = kexikwmailmergebase.cpp kexikwmmconfigdlg.cpp kexikwmmconnection.cpp kexikwmailmergebase.skel
kwmailmerge_kexi_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) -no-undefined
kwmailmerge_kexi_la_LIBADD = $(LIB_KDEUI) $(top_builddir)/kexi/core/libkexicore.la $(top_builddir)/kword/libkwmailmerge_interface.la -lDCOP

INCLUDES= -I$(top_srcdir) $(KOFFICE_INCLUDES) -I$(top_srcdir)/kexi/ -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi/kexiDB $(all_includes)

METASOURCES = AUTO

servicesdir=$(kde_servicesdir)
services_DATA = kwserialletter_kexi.desktop
