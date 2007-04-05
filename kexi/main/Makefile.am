include $(top_srcdir)/kexi/Makefile.global

lib_LTLIBRARIES = libkeximain.la
libkeximain_la_SOURCES = \
	keximainwindowimpl.cpp \
	kexistatusbar.cpp \
	kexinamewidget.cpp kexinamedialog.cpp \
	kexinewstuff.cpp kexifinddialogbase.ui kexifinddialog.cpp

libkeximain_la_LDFLAGS = -no-undefined $(KDE_RPATH) $(all_libraries) \
	$(VER_INFO) -Wnounresolved

SUBDIRS = startup printing .

libkeximain_la_LIBADD =  $(top_builddir)/kexi/kexidb/libkexidb.la $(top_builddir)/kexi/core/libkexicore.la \
	$(top_builddir)/kexi/kexiutils/libkexiutils.la \
	$(top_builddir)/kexi/main/startup/libkeximainstartup.la \
	$(top_builddir)/kexi/main/printing/libkeximainprinting.la \
	$(top_builddir)/lib/koproperty/libkoproperty.la \
	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
	$(LIB_KFEEDBACK) $(LIB_KEXI_KMDI) $(LIB_KNEWSTUFF) $(LIB_KIO) $(LIB_KDEPRINT)

#disabled	../migration/libkeximigrate.la

INCLUDES = $(INC_KFEEDBACK) $(LIB_KEXI_KMDI_INCLUDES) \
	-I$(top_builddir)/lib/kofficeui -I$(top_srcdir)/lib/kofficeui \
	-I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/core \
	-I$(top_srcdir)/kexi/main \
	-I$(top_srcdir)/kexi/main/startup -I$(top_srcdir)/kexi/widget \
	-I$(top_srcdir)/kexi/migration -I$(top_srcdir)/lib -I$(top_srcdir)/lib/kofficecore \
	$(all_includes)

noinst_HEADERS = ksplitter.h keximainwindowimpl_p.h

METASOURCES = AUTO
