include $(top_srcdir)/kexi/Makefile.global

lib_LTLIBRARIES = libkexicore.la
libkexicore_la_SOURCES = kexi.cpp kexi_global.cpp \
	kexiactionproxy.cpp kexisharedactionhost.cpp \
	kexiproject.cpp keximainwindow.cpp kexidialogbase.cpp kexiviewbase.cpp \
	kexipartmanager.cpp kexipartinfo.cpp kexipartitem.cpp kexipart.cpp \
	kexibrowser.cpp kexibrowseritem.cpp \
	kexiprojectdata.cpp kexistatusbar.cpp kexicontexthelp.cpp kexiinternalpart.cpp \
	kexidragobjects.cpp

# kexiprojectconnectiondata.cpp

libkexicore_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) -Wnounresolved -no-undefined

SUBDIRS = startup filters .

libkexicore_la_LIBADD = $(LIB_KEXI_KMDI) \
	../kexidb/libkexidb.la \
	./startup/kexicorestartup.la \
	./filters/libkexifilter.la \
	$(LIB_KOFFICEUI)

INCLUDES = $(KOFFICE_INCLUDES) \
	$(LIB_KEXI_KMDI_INCLUDES) \
	-I$(top_srcdir)/kexi \
	-I$(top_srcdir)/kexi/core/projectWizard \
	-I$(top_srcdir)/kexi/3rdparty/uuid \
	-I./startup \
	-I$(top_srcdir)/kexi/core/startup \
	-I$(top_srcdir)/lib/kofficeui \
	$(all_includes)

METASOURCES = AUTO

