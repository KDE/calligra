####### General stuff

INCDIR 	= -I/usr/X11R6/include -I$(QTDIR)/include -I$(KDEDIR)/include -I../../lib/kofficeui
LDFLAGS = -s -L/usr/X11R6/lib -L/usr/lib -L$(KDEDIR)/lib -L$(QTDIR)/lib -L../../lib/kofficeui -lkdecore -lkdeui -lqt -lm -lXext -lX11 -lkofficeui
CFLAGS	=  

MOC	= moc
CC 	= gcc

####### Files

SOURCES = 	main.cc autoformEdit.cc editWidget.cc drawWidget.cc atfinterpreter.cc\
		pntInsDia.cc afchoose.cc saveAsDia.cc

HEADERS	=	autoformEdit.h editWidget.h drawWidget.h atfinterpreter.h\
		pntInsDia.h afchoose.h saveAsDia.h 

SRCMETA =	autoformEdit.moc editWidget.moc drawWidget.moc atfinterpreter.moc\
		pntInsDia.moc afchoose.moc saveAsDia.moc

OBJECTS =	main.o autoformEdit.o editWidget.o drawWidget.o atfinterpreter.o\
		pntInsDia.o afchoose.o saveAsDia.o

TARGET	=	kautoformEdit

####### Implicit rules

.SUFFIXES: .cc

.cc.o:
	$(CC) -g -c $(CFLAGS) $(DEFS) $(INCDIR) $(HELP) $<
.c.o:
	$(CC) -g -c $(CFLAGS) $(DEFS) $(INCDIR) $(HELP) $<

####### Build rules

install: all

all: symlinks $(TARGET)

symlinks:	
	ln -sf ../atfinterpreter.cc atfinterpreter.cc
	ln -sf ../atfinterpreter.h atfinterpreter.h
	ln -sf ../afchoose.cc afchoose.cc
	ln -sf ../afchoose.h afchoose.h

$(TARGET): $(SOURCES) $(SRCMETA) $(OBJECTS) $(HEADER)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

depend:
	@makedepend -I$(INCDIR) $(SOURCES) 2> /dev/null


install-exec-local:
	$(mkinstalldirs) $(bindir)
	mv autoformEdit kautoformEdit
	$(INSTALL_SCRIPT) kautoformEdit $(bindir)

clean:	
	rm -f *.moc *.o
	rm -f kautoformEdit

uninstall:
	rm -f $(bindir)/kautoformEdit


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

# DO NOT DELETE

