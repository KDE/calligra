lib_LTLIBRARIES = libkexicore.la
libkexicore_la_SOURCES = kexi_global.cpp \
	kexiproject.cpp keximainwindow.cpp kexidialogbase.cpp \
	kexistartupdlgui.ui kexistartupdlg.cpp kexiprojectconnectiondata.cpp \
	kexipartmanager.cpp kexipartinfo.cpp kexipartitem.cpp kexipart.cpp \
	kexibrowser.cpp kexibrowseritem.cpp \
	kexiprojectdata.cpp

libkexicore_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) -Wnounresolved -no-undefined

SUBDIRS = startup .

libkexicore_la_LIBADD = $(LIB_KEXI_KMDI) \
	../kexidb/libkexidb.la \
	./startup/kexicorestartup.la

INCLUDES = $(KOFFICE_INCLUDES) \
	$(LIB_KEXI_KMDI_INCLUDES) \
	-I$(top_srcdir)/kexi \
	-I$(top_srcdir)/kexi/core/projectWizard \
	-I$(top_srcdir)/kexi/3rdparty/uuid \
	$(all_includes)

METASOURCES = AUTO

include $(top_srcdir)/kexi/Makefile.global
