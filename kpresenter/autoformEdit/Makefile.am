####### General stuff

INCLUDES = $(KOFFICE_INCLUDES) $(all_includes) 
LDFLAGS =  $(all_libraries) $(KDE_RPATH)
LDADD =  libaf.a $(LIB_KOFFICEUI)

#bin_PROGRAMS = kautoformEdit

####### Files

noinst_LIBRARIES = libaf.a

libaf_a_SOURCES = atfinterpreter.cc afchoose.cc

#kautoformEdit_SOURCES = main.cc autoformEdit.cc editWidget.cc \
#	drawWidget.cc pntInsDia.cc saveAsDia.cc

#noinst_HEADERS	= autoformEdit.h editWidget.h drawWidget.h atfinterpreter.h \
#		pntInsDia.h afchoose.h saveAsDia.h 

METASOURCES = atfinterpreter.moc afchoose.moc
#autoformEdit.moc editWidget.moc drawWidget.moc \
#	atfinterpreter.moc pntInsDia.moc afchoose.moc saveAsDia.moc

####### Build rules

######## Meta objects 

