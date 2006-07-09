####### General stuff

INCLUDES= -I$(srcdir) -I$(srcdir)/libqpro -I$(top_srcdir)/kspread \
	$(KOFFICE_INCLUDES) \
	$(KOTEXT_INCLUDES) $(all_includes)

####### Files

SUBDIRS = libqpro

kde_module_LTLIBRARIES = libqproimport.la

libqproimport_la_SOURCES = qproimport.cc qproformula.cc

libqproimport_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libqproimport_la_LIBADD  = $(KOFFICE_LIBS) ../../../kspread/libkspreadcommon.la \
	libqpro/src/libqpro.la

METASOURCES = AUTO

service_DATA = kspread_qpro_import.desktop
servicedir = $(kde_servicesdir)
