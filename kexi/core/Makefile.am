include $(top_srcdir)/kexi/Makefile.global

lib_LTLIBRARIES = libkexicore.la
libkexicore_la_SOURCES = kexi_global.cpp kexi.cpp \
	keximainwindow.cpp \
	kexidbconnectionset.cpp kexiprojectset.cpp \
	kexiactionproxy.cpp kexisharedactionhost.cpp \
	kexiproject.cpp kexidialogbase.cpp kexiviewbase.cpp \
	kexipartmanager.cpp kexipartinfo.cpp kexipartitem.cpp kexipart.cpp \
	kexiprojectdata.cpp kexiinternalpart.cpp \
	kexidragobjects.cpp \
	kexiuseraction.cpp kexiuseractionmethod.cpp \
	kexistartupdata.cpp kexiguimsghandler.cpp \
	kexidataiteminterface.cpp kexievents.cpp \
	kexidbshortcutfile.cpp \
	kexiscripting.cpp

#kexipartdatasource.cpp

if compile_kross_plugin
  KROSS_DEFINES=-DKEXI_KROSS_SUPPORT
  KROSS_INCLUDE=-I$(top_srcdir)/kexi/scriptingcore
  KROSS_LIB=$(top_builddir)/kexi/scriptingcore/main/libkrossmain.la
endif

AM_CPPFLAGS=$(KROSS_DEFINES)

libkexicore_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) $(VER_INFO) -Wnounresolved -no-undefined

SUBDIRS = .

libkexicore_la_LIBADD = $(LIB_KEXI_KMDI) \
	$(top_builddir)/kexi/kexiutils/libkexiutils.la \
	$(top_builddir)/kexi/kexidb/libkexidb.la $(top_builddir)/kexi/kexidb/parser/libkexidbparser.la \
	$(top_builddir)/lib/koproperty/libkoproperty.la \
	$(KROSS_LIB)

INCLUDES = $(LIB_KEXI_KMDI_INCLUDES) \
	-I$(top_srcdir)/lib/kofficecore \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/kexi \
	$(KROSS_INCLUDE) \
	$(all_includes)

noinst_HEADERS = kexiactionproxy_p.h kexicontexthelp_p.h kexisharedactionhost_p.h

METASOURCES = AUTO
