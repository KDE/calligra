include $(top_srcdir)/kexi/Makefile.global

lib_LTLIBRARIES = libkexicore.la
libkexicore_la_SOURCES = kexi_global.cpp kexi.cpp kexivalidator.cpp \
	keximainwindow.cpp \
	kexidbconnectionset.cpp kexiprojectset.cpp \
	kexiactionproxy.cpp kexisharedactionhost.cpp \
	kexiproject.cpp kexidialogbase.cpp kexiviewbase.cpp \
	kexipartmanager.cpp kexipartinfo.cpp kexipartitem.cpp kexipart.cpp \
	kexiprojectdata.cpp kexiinternalpart.cpp \
	kexidragobjects.cpp \
	kexiproperty.cpp kexipropertybuffer.cpp kexipartdatasource.cpp \
	kexiuseraction.cpp kexiuseractionmethod.cpp \
	kexistartupdata.cpp kexiguimsghandler.cpp \
	kexidataiteminterface.cpp kexievents.cpp

noinst_HEADERS = kexi_p.h kexiactionproxy_p.h kexicontexthelp_p.h kexisharedactionhost_p.h

libkexicore_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) $(VER_INFO) -Wnounresolved -no-undefined

SUBDIRS = .

libkexicore_la_LIBADD = $(LIB_KEXI_KMDI) \
	../kexidb/libkexidb.la ../kexidb/parser/libkexidbparser.la

INCLUDES = $(LIB_KEXI_KMDI_INCLUDES) \
	-I$(top_srcdir)/lib/kofficeui \
	-I$(top_srcdir)/kexi \
	$(all_includes)

METASOURCES = AUTO
