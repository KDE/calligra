INCLUDES  = $(MICO_INCLUDES) $(all_includes) -I$(idldir) -I.. -I../share
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
FILTER_LIBS = ../filter/libfilter.a
XML_LIBS = ../xmlutils/libxml.a
KSHARE_LIBS = ../share/libkillu.a
LDADD = $(KSHARE_LIBS) $(XML_LIBS) $(FILTER_LIBS) \
	-lkofficeui -lkofficecore -lkoml \
	 -lkfile -lkfm -lop -lkom -lqt -lkdeui -lkdecore \
	$(LIBMICO) -lXext -lX11 $(LIBSOCKET)

####### Files

bin_PROGRAMS = killustrator.bin

killustrator_bin_SOURCES = KIllustrator_main.cc \
	KIllustrator_doc.cc \
	KIllustrator_shell.cc \
	KIllustrator_view.cc \
	KIllustrator.cc 

noinst_HEADERS = KIllustrator_main.h \
	KIllustrator_doc.h \
	KIllustrator_shell.h \
	KIllustrator_view.h \
	KIllustrator.h

SRCMETA = KIllustrator_main.moc \
	KIllustrator_shell.moc \
	KIllustrator_doc.moc \
	KIllustrator_view.moc

idl_DATA = KIllustrator.idl
idl_HEADERS = KIllustrator.h

kdelnk_DATA = killustrator_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk

SUBDIRS = 

####### Implicit rules

SUFFIXES = .h .moc

.h.moc: 
	$(MOC) $< -o $@

####### Build rules

install-exec-local:
	$(INSTALL_SCRIPT) killustrator $(bindir)/killustrator

KIllustrator.h: KIllustrator.idl
	$(IDL) --query-server-for-narrow -I$(idldir) KIllustrator.idl


KIllustrator.cc: KIllustrator.h
KIllustrator_main.cc: KIllustrator.h KIllustrator_main.moc
KIllustrator_doc.cc: KIllustrator.h KIllustrator_doc.moc
KIllustrator_shell.cc: KIllustrator.h KIllustrator_shell.moc
KIllustrator_view.cc: KIllustrator.h KIllustrator_view.moc
