include $(top_srcdir)/kexi/Makefile.global

lib_LTLIBRARIES = libkexiimportwizard.la

libkexiimportwizard_la_SOURCES =  kexiimportwizard.cpp kexiimportwizardformbase.ui\
	kexiimportwizardform.cpp
libkexiimportwizard_la_LDFLAGS = $(all_libraries)  --no-unresolved
libkexiimportwizard_la_LIBADD = ../../../core/libkexicore.la


INCLUDES= $(KOFFICE_INCLUDES) -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
		-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/tableview \
		-I$(top_srcdir)/kexi/kexiDB -I$(top_srcdir)/kexi/plugins/tables $(all_includes)

METASOURCES = AUTO

