####### General stuff

INCLUDES = $(KOFFICE_INCLUDES) $(all_includes) $(MICO_INCLUDES) -I/usr/include/pgsql
LDFLAGS = $(all_libraries) $(MICO_LDFLAGS) -lpq++ -version-info 1:0:0
#LDFLAGS = $(all_libraries) $(MICO_LDFLAGS) -version-info 1:0:0
libkdb_la_LIBADD = ../../lib/kofficeui/libkofficeui.la

####### Files

lib_LTLIBRARIES = libkdb.la

libkdb_la_SOURCES = kdbrelation.cpp kdbrecordviewitem.cpp kdbrecordview.cpp \
	kdbpickdate.cpp kdbpgbase.cpp kdbmoneyedit.cpp kdbfieldset.cpp \
	kdbfieldedit.cpp kdbdataset.cpp kdbdatafield.cpp kdbdatabase.cpp \
	kdbcriteria.cpp kdbcalendar.cpp kdbBaseType.cpp kdb.cpp

noinst_HEADERS = kdb.h kdbBaseType.h kdbcalendar.h kdbcriteria.h kdbdatabase.h \
	kdbdatafield.h kdbdataset.h  kdbfieldedit.h kdbfieldset.h kdbmoneyedit.h \
	kdbpgbase.h kdbpickdate.h kdbrecordview.h kdbrecordviewitem.h kdbrelation.h

METASOURCES = kdbcalendar.moc kdbcriteria.moc kdbdatabase.moc kdbdatafield.moc \
	kdbdataset.moc kdbfieldedit.moc kdbmoneyedit.moc kdbpickdate.moc \
	kdbrecordview.moc kdbrelation.moc

IDL_FLAGS = --query-server-for-narrow --relative-paths --any -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(srcdir) -I$(idldir)

