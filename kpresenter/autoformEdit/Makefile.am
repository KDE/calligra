####### General stuff

INCLUDES = $(MICO_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes) -I$(idldir)
LDFLAGS =  $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD =  libaf.a $(KOFFICE_LIBS) -lkpartsui -lkparts -lkom $(LIB_KIO) $(LIBMICO) $(LIB_KFILE) -lm

bin_PROGRAMS = kautoformEdit

####### Files

noinst_LIBRARIES = libaf.a

libaf_a_SOURCES = atfinterpreter.cc afchoose.cc

kautoformEdit_SOURCES = main.cc autoformEdit.cc editWidget.cc \
	drawWidget.cc pntInsDia.cc saveAsDia.cc

noinst_HEADERS	= autoformEdit.h editWidget.h drawWidget.h atfinterpreter.h \
		pntInsDia.h afchoose.h saveAsDia.h 

METASOURCES = autoformEdit.moc editWidget.moc drawWidget.moc \
	atfinterpreter.moc pntInsDia.moc afchoose.moc saveAsDia.moc

####### Build rules

######## Meta objects 

