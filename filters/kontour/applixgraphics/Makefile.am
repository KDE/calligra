####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libapplixgraphicimport_la_LDFLAGS	= -module -avoid-version -no-undefined 

####### Files

kde_module_LTLIBRARIES = libapplixgraphicimport.la

libapplixgraphicimport_la_SOURCES = applixgraphicimport.cc applixgraphicimport_factory.cc
libapplixgraphicimport_la_LIBADD = $(KOFFICE_LIBS)
noinst_HEADERS = applixgraphicimport.h applixgraphicimport_factory.h

METASOURCES = AUTO

service_DATA = killustrator_applixgraphic_import.desktop
servicedir = $(kde_servicesdir)

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/kontourapplixgraphicsfilter.pot
