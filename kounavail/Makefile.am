INCLUDES  = $(KOFFICE_INCLUDES) $(all_includes)

## The part
kde_module_LTLIBRARIES = libkounavailpart.la
libkounavailpart_la_SOURCES = kounavail.cc
libkounavailpart_la_LDFLAGS = $(KDE_PLUGIN)
libkounavailpart_la_LIBADD = $(LIB_KOFFICECORE)

METASOURCES = AUTO
noinst_HEADERS =  kounavail.h

kdelnk_DATA = kounavail.desktop
kdelnkdir = $(kde_servicesdir)

messages:
	$(XGETTEXT) *.cc -o $(podir)/kounavail.pot
