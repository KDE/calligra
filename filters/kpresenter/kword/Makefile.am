INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)

kde_module_LTLIBRARIES = libkprkword.la

libkprkword_la_SOURCES = kprkword.cc
libkprkword_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libkprkword_la_LIBADD = $(KOFFICE_LIBS) 

METASOURCES = AUTO

service_DATA = kprkword.desktop
servicedir = $(kde_servicesdir)
