INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)

kde_module_LTLIBRARIES = libkprkword.la

libkprkword_la_SOURCES = kprkword.cc kprkword_factory.cc
libkprkword_la_LDFLAGS	= -module -avoid-version -no-undefined
libkprkword_la_LIBADD = $(KOFFICE_LIBS)

METASOURCES = AUTO

service_DATA = kprkword.desktop
servicedir = $(kde_servicesdir)

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/kpresenterkwordfilter.pot
