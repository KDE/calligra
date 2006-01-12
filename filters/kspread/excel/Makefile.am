####### General stuff

INCLUDES= -I$(srcdir) -I$(top_srcdir)/kspread $(KOFFICE_INCLUDES) \
	$(KOTEXT_INCLUDES) $(all_includes)

SUBDIRS = sidewinder import

####### Files

METASOURCES = AUTO

servicedir = $(kde_servicesdir)

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/kspreadexcelfilter.pot
