####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libapplixgraphicimport_la_LDFLAGS	= -module -avoid-version -no-undefined $(all_libraries)

####### Files

kde_module_LTLIBRARIES = libapplixgraphicimport.la

libapplixgraphicimport_la_SOURCES = applixgraphicimport.cc
libapplixgraphicimport_la_LIBADD = $(KOFFICE_LIBS)
noinst_HEADERS = applixgraphicimport.h

METASOURCES = AUTO

service_DATA = kontour_applixgraphic_import.desktop
servicedir = $(kde_servicesdir)

messages: rc.cpp
	$(XGETTEXT) *.cc *.cpp -o $(podir)/kontourapplixgraphicsfilter.pot
