SUBDIRS = pics

INCLUDES  = $(MICO_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes) -I$(idldir)
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = $(LIB_KOFFICEUI) $(LIB_KIMGIO)

####### Files

bin_PROGRAMS = kimage.bin

kimage_bin_SOURCES = kimage_main.cc kimage_shell.cc kimage_doc.cc \
		     kimage.cc kimage_view.cc

noinst_HEADERS = kimage_main.h kimage_shell.h kimage_doc.h \
	         kimage.h kimage_view.h

METASOURCES = kimage_main.moc kimage_shell.moc kimage_doc.moc kimage_view.moc

idl_DATA = kimage.idl
idl_HEADERS = kimage.h

DISTCLEANFILES = $(idl_DATA:.idl=.cc) $(idl_HEADERS) $(METASOURCES)

kdelnk_DATA = kimage_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

####### Implicit rules

####### Build rules

IDL_FLAGS = --query-server-for-narrow --relative-paths -I$(top_srcdir)/lib/kofficecore -I$(top_srcdir)/lib/store -I$(interfacedir) -I$(idldir)

install-exec-local:
	$(INSTALL_SCRIPT) $(srcdir)/kimage $(bindir)/kimage

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot -x $(includedir)/corba.pot  *.cc && mv messages.po ../po/kimage.pot

kimage.h: kimage.idl
	$(IDL) $(IDL_FLAGS) $(srcdir)/kimage.idl

kimage.cc: kimage.h
kimage_doc.cc: kimage.h
kimage_main.cc: kimage.h
kimage_shell.cc: kimage.h
kimage_view.cc: kimage.h
