CC = g++
CFLAGS = -pg -g -Wall -Wno-unused
LDFLAGS = -pg

#CFLAGS = -O2 -Wall
#LDFLAGS =
# -funroll-all-loops


OBJ = brush.o channel.o layer.o canvas.o numSlider.o misc.o layerlist.o main.o

METASRC = numSlider.mcc layer.mcc canvas.mcc

INCLUDE = -I/usr/local/qt/include
#INCLUDE = -I/usr/include/qt

LIBS =  -lm -L/usr/local/qt/lib -lqt -L /usr/X11R6/lib -lXext\
	 -lqimgio -ljpeg -lpng -ltiff


all: canvas

.PHONY: clean dep 
clean:
	rm -f *.o *~ *.mcc core .depend gmon.out testFastPixmap canvas

canvas: $(METASRC) $(OBJ) 
	$(CC) $(LDFLAGS) -o canvas $(OBJ) $(LIBS) 

testFastPixmap: testFastPixmap.o 
	$(CC) -o testFastPixmap testFastPixmap.o $(LIBS) 

.SUFFIXES:
.SUFFIXES: .c .o .cc


%.mcc: %.h
	moc -i $< -o $@

.cc.o:
	$(CC) -c ${INCLUDE} $(CFLAGS) $(GGDB) $(PG) $< 

.c.o:
	$(CC) -c ${INCLUDE} $(CFLAGS) $(GGDB) $(PG) $< 

dep:
	gcc -MM -MG ${INCLUDE} *.cc > .depend

-include .depend
