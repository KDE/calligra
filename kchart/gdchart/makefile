GDC_INCL = .

CC=g++ -g

all: gdc_samp1 # gdc_samp2 gdc_pie_samp

# compile the samples
gdc_samp1: gdc.o gdchart.o price_conv.o gdc_samp1.o params.o init.o
	$(CC) -o gdc_samp1 gdc.o gdchart.o price_conv.o gdc_samp1.o params.o init.o -L$(QTDIR)/lib -lqt -lm

gdc_samp1.o: $(GDC_INCL)/gdc.h $(GDC_INCL)/gdchart.h gdc_samp1.cpp
	$(CC) -I$(GDC_INCL) -c gdc_samp1.cpp -I$(QTDIR)/include

gdc_samp2: gdc.o gdchart.o price_conv.o gdc_samp2.o params.o init.o
	$(CC) -o gdc_samp2 gdc.o gdchart.o price_conv.o gdc_samp2.o params.o init.o -L$(QTDIR)/lib -lqt -lm

gdc_samp2.o: $(GDC_INCL)/gdc.h $(GDC_INCL)/gdchart.h gdc_samp2.cpp
	$(CC) -I$(GDC_INCL) -c gdc_samp2.cpp -I$(QTDIR)/include

gdc_pie_samp: gdc.o gdc_pie.o price_conv.o gdc_pie_samp.o params.o init.o
	$(CC) -o gdc_pie_samp gdc.o gdc_pie.o price_conv.o gdc_pie_samp.o params.o init.o -L$(QTDIR)/lib -lqt -lm

gdc_pie_samp.o: $(GDC_INCL)/gdc.h $(GDC_INCL)/gdcpie.h gdc_pie_samp.cpp
	$(CC) -I$(GDC_INCL) -c gdc_pie_samp.cpp -I$(QTDIR)/include

# --- compile the lib ---
price_conv.o: price_conv.cpp
	$(CC) -c price_conv.cpp

gdc.o: gdc.cpp gdc.h
	$(CC) -I$(GDC_INCL) -c gdc.cpp -I$(QTDIR)/include

gdc_pie.o: $(GDC_INCL)/gdc.h $(GDC_INCL)/gdcpie.h gdc_pie.cpp
	$(CC) -I$(GDC_INCL) -c gdc_pie.cpp -I$(QTDIR)/include

gdchart.o: $(GDC_INCL)/gdc.h $(GDC_INCL)/gdchart.h gdchart.cpp
	$(CC) -I$(GDC_INCL) -c gdchart.cpp -I$(QTDIR)/include

params.o: $(GDC_INCL)/gdc.h $(GDC_INCL)/gdchart.h params.cpp
	$(CC) -I$(GDC_INCL) -c params.cpp -I$(QTDIR)/include

init.o: $(GDC_INCL)/gdc.h $(GDC_INCL)/gdchart.h init.cpp
	$(CC) -I$(GDC_INCL) -c init.cpp -I$(QTDIR)/include


# --- clean ---
clean:
	rm -f *.o gdc_samp1 gdc_samp2 gdc_pie_samp
	cd $(GD_LIB) ; make -f Makefile clean

