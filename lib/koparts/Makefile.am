####### General stuff

INCLUDES= -I$(top_srcdir) $(all_includes)
LDFLAGS	= $(all_libraries)  -version-info 1:0:0
libkoparts_la_LIBADD  = $(LIB_KDEUI)

####### Files

lib_LTLIBRARIES = libkoparts.la

libkoparts_la_SOURCES = container.cpp  handler.cpp  part.cpp    shell.cpp    view.cpp \
		       frame.cpp      plugin.cpp  toolbox.cpp   \
		kcoloractions.cpp

include_HEADERS = container.h  handler.h  part.h    shell.h    view.h \
		  frame.h      plugin.h  toolbox.h  \
		kcoloractions.h

METASOURCES = AUTO

