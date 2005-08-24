include $(top_srcdir)/kexi/Makefile.global

lib_LTLIBRARIES = libkeximain.la
libkeximain_la_SOURCES = \
	keximainwindowimpl.cpp \
	kexibrowser.cpp kexibrowseritem.cpp \
	kexistatusbar.cpp \
	kexinamewidget.cpp kexinamedialog.cpp \
	kexinewstuff.cpp \
	kexiscripting.cpp

# kexiprojectconnectiondata.cpp

if compile_kross_plugin
  KROSS_DEFINES=-DKEXI_KROSS_SUPPORT
  KROSS_INCLUDE=-I$(top_srcdir)/kexi/scripting
  KROSS_LIB=$(top_builddir)/kexi/scripting/main/libkrossmain.la
endif

libkeximain_la_LDFLAGS = -no-undefined $(KDE_RPATH) $(all_libraries) \
	$(VER_INFO) -Wnounresolved

AM_CPPFLAGS=$(KROSS_DEFINES)

SUBDIRS = startup .

libkeximain_la_LIBADD =  $(top_builddir)/kexi/kexidb/libkexidb.la $(top_builddir)/kexi/core/libkexicore.la \
	$(top_builddir)/kexi/main/startup/libkeximainstartup.la \
	$(top_builddir)/lib/koproperty/libkoproperty.la \
	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
	$(LIB_KFEEDBACK) $(LIB_KEXI_KMDI) $(LIB_KNEWSTUFF) $(LIB_KIO) \
	$(KROSS_LIB)

#disabled	../migration/libkeximigrate.la

INCLUDES = -I$(INC_KFEEDBACK) $(LIB_KEXI_KMDI_INCLUDES) \
	-I$(top_srcdir)/lib/kofficeui -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/core \
	-I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi/main -I$(top_srcdir)/kexi/main/projectWizard \
	-I$(top_srcdir)/kexi/main/startup -I$(top_srcdir)/kexi/main/startup -I$(top_srcdir)/kexi/widget \
	-I$(top_srcdir)/kexi/migration -I$(top_srcdir)/lib -I$(top_srcdir)/lib/kofficecore \
	$(KROSS_INCLUDE) \
	$(all_includes)

noinst_HEADERS = ksplitter.h

METASOURCES = AUTO
