####### General stuff

KDE_CXXFLAGS = $(USE_RTTI)
INCLUDES= -I$(top_srcdir) $(all_includes)
libkoparts_la_LIBADD  = $(LIB_KDEUI)

####### Files

lib_LTLIBRARIES = libkoparts.la

libkoparts_la_SOURCES = container.cpp  handler.cpp  part.cpp    shell.cpp    view.cpp \
		       frame.cpp      plugin.cpp  toolbox.cpp   \
		kcoloractions.cpp
libkoparts_la_LDFLAGS = $(all_libraries)  -version-info 1:0:0 -no-undefined

include_HEADERS = container.h  handler.h  part.h    shell.h    view.h \
		  frame.h      plugin.h  toolbox.h  \
		kcoloractions.h

METASOURCES = AUTO

