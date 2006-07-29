# This tool is disabled. Not useful and not ported to kspell2.

#INCLUDES = -I$(srcdir)/../../lib/kofficecore  $(all_includes)
#libkspelltool_la_LIBADD = ../../lib/kofficecore/libkofficecore.la 

####### Files

#kde_module_LTLIBRARIES = libkspelltool.la

#libkspelltool_la_SOURCES = main.cc

#libkspelltool_la_LDFLAGS = $(all_libraries) -avoid-version -module -no-undefined

#noinst_HEADERS = main.h

#libkspelltool_la_METASOURCES = main.moc

#service_DATA = kspelltool.desktop
#servicedir = $(kde_servicesdir)

install-data-local: uninstall.desktop
	$(mkinstalldirs) $(DESTDIR)$(kde_servicesdir)
	$(INSTALL_DATA) $(srcdir)/uninstall.desktop $(DESTDIR)$(kde_servicesdir)/kspelltool.desktop

