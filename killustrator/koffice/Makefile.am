INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes) -I../share -I../filter -I../

killustrator_LDFLAGS = $(all_libraries) $(KDE_RPATH)
killustrator_LDADD = libkillustrator.la

libkillustrator_la_LDFLAGS = -avoid-version -module

libkillustrator_la_LIBADD = $(all_libraries) ../share/libkillu.la ../xmlutils/libxml.la ../filter/libfilter.la ../../lib/kofficeui/libkofficeui.la

####### Files

bin_PROGRAMS = killustrator
lib_LTLIBRARIES = libkillustrator.la

libkillustrator_la_SOURCES = KIllustrator_doc.cc \
	KIllustrator_shell.cc KIllustrator_view.cc \
	InsertPartTool.cc GPart.cc KIllustrator_factory.cc

killustrator_SOURCES = main.cc

noinst_HEADERS = KIllustrator_shell.h KIllustrator_doc.h \
		 KIllustrator_view.h InsertPartTool.h GPart.h \
		 KIllustrator_factory.h

libkillustrator_la_METASOURCES = KIllustrator_shell.moc KIllustrator_doc.moc \
				 KIllustrator_view.moc InsertPartTool.moc GPart.moc \
				 KIllustrator_factory.moc

kdelnk_DATA = killustrator.desktop
kdelnkdir = $(kde_appsdir)/Applications/KOffice

kdemime_DATA = x-killustrator.desktop
kdemimedir = $(kde_mimedir)/application

rcdir = $(kde_datadir)/killustrator
rc_DATA = KIllustrator.rc KIllustrator_shell.rc

# SUBDIRS =  . plugins pics dtd toolbar

