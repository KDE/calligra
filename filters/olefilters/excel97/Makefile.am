####### General stuff

INCLUDES= -I$(srcdir) -I$(srcdir)/../lib $(all_includes)

####### Files

noinst_LTLIBRARIES = libexcel97.la

libexcel97_la_SOURCES = helper.cc excelfilter.cc handler.cc worker.cc
libexcel97_la_LDFLAGS = $(all_libraries)

noinst_HEADERS = excelfilter.h handler.h worker.h helper.h definitions.h

METASOURCES = AUTO

install-data-local: uninstall.desktop
	$(INSTALL_DATA) $(srcdir)/uninstall.desktop $(DESTDIR)$(kde_servicesdir)/ole_excel97_import.desktop

#service_DATA = ole_excel97_import.desktop
#servicedir = $(kde_servicesdir)
