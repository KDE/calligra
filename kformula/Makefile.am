INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) $(all_includes)
KDE_CXXFLAGS = $(USE_RTTI)
kformula_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kformula_LDADD = libkformula.la

libkformula_la_LDFLAGS = -module -avoid-version

libkformula_la_LIBADD = ../lib/kofficeui/libkofficeui.la

####### Files

bin_PROGRAMS = kformula
lib_LTLIBRARIES = libkformula.la

libkformula_la_SOURCES = kformula_shell.cc kformula_doc.cc \
	kformula_view.cc kformula_factory.cc BasicElement.cc \
	RootElement.cc TextElement.cc BracketElement.cc FractionElement.cc\
	MatrixElement.cc PrefixedElement.cc formula_container.cc matrixwidget.cc

kformula_SOURCES = main.cc

noinst_HEADERS =  kformula_shell.h kformula_doc.h \
		  kformula_view.h kformula_factory.h  formuladef.h \
	          BasicElement.h RootElement.h TextElement.h\
		  BracketElement.h FractionElement.h MatrixElement.h PrefixedElement.h \
		  matrixwidget.h formula_container.h


libkformula_la_METASOURCES = kformula_shell.moc kformula_doc.moc kformula_view.moc kformula_factory.moc \
			     matrixwidget.moc
#			     formula_container.moc

rcdir = $(kde_datadir)/kformula
rc_DATA = kformula.rc kformula_shell.rc


kdelnk_DATA = kformula.desktop
kdelnkdir = $(kde_appsdir)/Applications/KOffice

kdemime_DATA = x-kformula.desktop
kdemimedir = $(kde_mimedir)/application

SUBDIRS =  pics

messages:
#	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot *.cc && mv messages.po $(podir)/kformula.pot


