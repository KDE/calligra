####### General stuff

INCLUDES = $(MICO_INCLUDES) -I../../lib/kofficecore -I../../lib/store -I../../lib/kofficeui \
	-I../../lib/koml $(all_includes) -I$(idldir)
LDFLAGS =  $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD =  ../../lib/kofficeui/libkofficeui.la ../../lib/store/libkstore.la \
	../../lib/kofficecore/libkofficecore.la ../../lib/koml/libkoml.la \
	-lkpartsui -lkparts -lkom -lkfile -lkfm -lkdecore -lkdeutil -lkdeui -lqt -lm -lXext -lX11 \
	$(LIBMICO) $(LIBSOCKET) 

bin_PROGRAMS = kautoformEdit

####### Files

kautoformEdit_SOURCES = main.cc autoformEdit.cc editWidget.cc \
	drawWidget.cc atfinterpreter.cc pntInsDia.cc afchoose.cc \
	saveAsDia.cc

noinst_HEADERS	= autoformEdit.h editWidget.h drawWidget.h atfinterpreter.h \
		pntInsDia.h afchoose.h saveAsDia.h 

SRCMETA = autoformEdit.moc editWidget.moc drawWidget.moc atfinterpreter.moc \
	pntInsDia.moc afchoose.moc saveAsDia.moc

####### Build rules


all:  $(TARGET)

######## Meta objects 

SUFFIXES = .h .moc 

.h.moc:
	$(MOC) $< -o $@      

afchoose.cc: afchoose.moc
atfinterpreter.cc: atfinterpreter.moc
saveAsDia.cc: saveAsDia.moc      
pntInsDia.cc: pntInsDia.moc
autoformEdit.cc: autoformEdit.moc
drawWidget.cc: drawWidget.moc
editWidget.cc: editWidget.moc
