include $(top_srcdir)/kexi/Makefile.global

lib_LTLIBRARIES = libkexicore.la
libkexicore_la_SOURCES = kexi.cpp kexi_global.cpp \
	kexiactionproxy.cpp kexisharedactionhost.cpp \
	kexiproject.cpp keximainwindow.cpp kexidialogbase.cpp kexiviewbase.cpp \
	kexipartmanager.cpp kexipartinfo.cpp kexipartitem.cpp kexipart.cpp \
	kexibrowser.cpp kexibrowseritem.cpp \
	kexiprojectdata.cpp kexistatusbar.cpp kexiinternalpart.cpp \
	kexidragobjects.cpp createitemui.ui kexicreateitemdlg.cpp

# kexiprojectconnectiondata.cpp

libkexicore_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) -Wnounresolved -no-undefined

SUBDIRS = startup .

libkexicore_la_LIBADD = $(LIB_KEXI_KMDI) \
	../kexidb/libkexidb.la \
	./startup/libkexicorestartup.la \
	$(LIB_KOFFICEUI)

INCLUDES = $(LIB_KEXI_KMDI_INCLUDES) \
	-I$(top_srcdir)/lib/kofficeui \
	-I$(top_srcdir)/kexi \
	-I$(top_srcdir)/kexi/core/projectWizard \
	-I./startup \
	-I$(top_srcdir)/kexi/core/startup \
	$(all_includes)

METASOURCES = AUTO
