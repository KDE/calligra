####### General stuff

INCLUDES= -I$(srcdir)/../ $(all_includes)
LDFLAGS	= $(all_libraries)  -version-info 1:0:0
libkparts_la_LIBADD  = $(LIB_KDEUI)

####### Files

lib_LTLIBRARIES = libkparts.la

libkparts_la_SOURCES = container.cpp  handler.cpp  part.cpp    shell.cpp    view.cpp \
		       frame.cpp      plugin.cpp  toolbox.cpp   \
		kcoloractions.cpp

include_HEADERS = container.h  handler.h  part.h    shell.h    view.h \
		  frame.h      plugin.h  toolbox.h  \
		kcoloractions.h

libkparts_la_METASOURCES = container.moc  handler.moc  part.moc    shell.moc    view.moc \
			   frame.moc      plugin.moc  toolbox.moc  \
			kcoloractions.moc
