kde_module_LTLIBRARIES = kexihandler_script.la

kexihandler_script_la_SOURCES = kexiscripthandler.cpp kexiscripthandlerproxy.cpp \
kexiscripthandleritem.cpp kexiqsaeventhandler.cpp kexiqsahost.cpp kexiqsaeditor.cpp \
kexiqsaclasses.cpp
kexihandler_script_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
kexihandler_script_la_LIBADD = ../../core/libkexicore.la ../../kexiDB/libkexidb.la \
/usr/build/qt-copy/lib/libqsa.so ../../formeditor/libkformeditor.la

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

INCLUDES= $(KOFFICE_INCLUDES) -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
		-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/tableview \
		-I$(top_srcdir)/kexi/kexiDB -I$(top_srcdir)/kexi/formeditor $(all_includes)

METASOURCES = AUTO

