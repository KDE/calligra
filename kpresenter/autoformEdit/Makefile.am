####### General stuff

INCLUDES = $(all_includes) -I../../lib/kofficeui
LDFLAGS =  $(all_libraries)
LDADD =  ../../lib/kofficeui/libkofficeui.la \
	../../lib/kofficecore/libkofficecore.la ../../lib/koml/libkoml.la \
	-lkdecore -lkdeui -lqt -lm -lXext -lX11 -lkofficeui -lop -lkom \
	-lkfile -lkfm -lkoml

bin_PROGRAMS = kautoformEdit

####### Files

kautoformEdit_SOURCES = main.cc autoformEdit.cc editWidget.cc \
	drawWidget.cc ../atfinterpreter.cc pntInsDia.cc afchoose.cc \
	saveAsDia.cc

noinst_HEADERS	= autoformEdit.h editWidget.h drawWidget.h atfinterpreter.h\
		pntInsDia.h afchoose.h saveAsDia.h 

SRCMETA = autoformEdit.moc editWidget.moc drawWidget.moc atfinterpreter.moc\
	pntInsDia.moc afchoose.moc saveAsDia.moc

####### Build rules


all: symlinks $(TARGET)

symlinks:	
	ln -sf ../atfinterpreter.cc atfinterpreter.cc
	ln -sf ../atfinterpreter.h atfinterpreter.h
	ln -sf ../afchoose.cc afchoose.cc
	ln -sf ../afchoose.h afchoose.h

######## Meta objects 

autoformEdit.moc: autoformEdit.h
	$(MOC) autoformEdit.h -o autoformEdit.moc

editWidget.moc: editWidget.h
	$(MOC) editWidget.h -o editWidget.moc

drawWidget.moc: drawWidget.h
	$(MOC) drawWidget.h -o drawWidget.moc

atfinterpreter.moc: atfinterpreter.h
	$(MOC) atfinterpreter.h -o atfinterpreter.moc

pntInsDia.moc: pntInsDia.h
	$(MOC) pntInsDia.h -o pntInsDia.moc

ainfodia.moc: ainfodia.h
	$(MOC) ainfodia.h -o ainfodia.moc

afchoose.moc: afchoose.h
	$(MOC) afchoose.h -o afchoose.moc

saveAsDia.moc: saveAsDia.h
	$(MOC) saveAsDia.h -o saveAsDia.moc


afchoose.cc: afchoose.moc
atfinterpreter.cc: atfinterpreter.moc
backdia.cc: backdia.moc
graphobj.cc: graphobj.moc
kcharselect.cc: kcharselect.moc
kcharselectdia.cc: kcharselectdia.moc
kenumlistdia.cc: kenumlistdia.moc
kpresenter_doc.cc: kpresenter_doc.moc
kpresenter_main.cc: kpresenter_main.moc
kpresenter_shell.cc: kpresenter_shell.moc
kpresenter_view.cc: kpresenter_view.moc
ktextobject.cc: ktextobject.moc
page.cc: page.moc
styledia.cc: styledia.moc      
