INCLUDES= $(all_includes) $(KOFFICEUI_INCLUDES)

lib_LTLIBRARIES = libkotext.la
libkotext_la_SOURCES = qcomplextext.cpp qrichtext.cpp qrichtext_p.cpp \
        qstylesheet.cpp mocstuff.cpp \
	kozoomhandler.cc koborder.cc \
	kotextformat.cc koparagcounter.cc kostyle.cc kocommand.cc \
	kotextparag.cc kotextdocument.cc kotextformatter.cc koparaglayout.cc \
	kotextobject.cc kotextview.cc
libkotext_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -no-undefined
# We use kofficeui for koRect :)
libkotext_la_LIBADD = $(LIB_KOFFICEUI)

METASOURCES = AUTO

# include_HEADERS = \
#	kozoomhandler.h kotextformat.h koparagcounter.h kostyle.h \
#	kotextparag.h kotextdocument.h
