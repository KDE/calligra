####### General stuff

INCLUDES = $(KOFFICE_INCLUDES) $(all_includes)

####### Files
kde_module_LTLIBRARIES = libkwordkword1dot3import.la


libkwordkword1dot3import_la_SOURCES = ExportFilter.cc \
TagProcessing.cc KWEFStructures.cc \
ProcessDocument.cc KWEFKWordLeader.cc \
kword13document.cpp kword13parser.cpp kword13frameset.cpp kword13layout.cpp \
kword13paragraph.cpp kword13utils.cpp tempmain.cpp kword13import.cpp \
kword13oasisgenerator.cpp kword13formatone.cpp

libkwordkword1dot3import_la_LDFLAGS = $(all_libraries) -module -avoid-version -no-undefined
libkwordkword1dot3import_la_LIBADD  = $(KOFFICE_LIBS)

noinst_HEADERS = TagProcessing.h KWEFStructures.h \
ProcessDocument.h KWEFKWordLeader.h ExportFilter.h \
kword13document.h kword13parser.h kword13frameset.h kword13layout.h \
kword13paragraph.h kword13utils.h kword13import.h \
kword13oasisgenerator.h kword13formatone.h

METASOURCES = AUTO

service_DATA = kword_kword1dot3_import.desktop

servicedir = $(kde_servicesdir)

