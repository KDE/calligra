include $(top_srcdir)/kexi/Makefile.global

lib_LTLIBRARIES = libkeximain.la
libkeximain_la_SOURCES = \
	keximainwindowimpl.cpp \
	kexibrowser.cpp kexibrowseritem.cpp \
	kexistatusbar.cpp \
	kexinamewidget.cpp kexinamedialog.cpp \
	projectsettingsui.ui


# kexiprojectconnectiondata.cpp

libkeximain_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) -Wnounresolved -no-undefined

SUBDIRS = startup .

libkeximain_la_LIBADD = $(LIB_KEXI_KMDI) \
	../kexidb/libkexidb.la \
	../core/libkexicore.la \
	./startup/libkeximainstartup.la \
	../widget/propertyeditor/libkexipropertyeditor.la \
	../migration/libkeximigrate.la

INCLUDES = $(LIB_KEXI_KMDI_INCLUDES) \
	-I$(top_srcdir)/lib/kofficeui \
	-I$(top_srcdir)/kexi \
	-I$(top_srcdir)/kexi/core \
	-I$(top_builddir)/kexi/core \
	-I$(top_srcdir)/kexi/main \
	-I$(top_srcdir)/kexi/main/projectWizard \
	-I$(top_srcdir)/kexi/main/startup \
	-I$(top_builddir)/kexi/main/startup \
	-I$(top_srcdir)/kexi/widget/propertyeditor \
	-I$(top_srcdir)/kexi/migration \
	$(all_includes)

METASOURCES = AUTO
