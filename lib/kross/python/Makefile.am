include $(top_srcdir)/lib/kross/Makefile.global

kde_module_LTLIBRARIES = krosspython.la

krosspython_la_LDFLAGS = $(LIBPYTHON) $(PYTHONLIB) -module $(KDE_PLUGIN) $(all_libraries) $(VER_INFO)

krosspython_la_SOURCES = \
    pythonextension.cpp \
    pythonmodule.cpp \
    pythonobject.cpp \
    pythonscript.cpp \
    pythonsecurity.cpp \
    pythoninterpreter.cpp

krosspython_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KPARTS) $(LIB_KFILE) $(LIB_KDEUI) $(LIB_KDECORE) \
    CXX/libkrosspythoncxx.la \
    ../api/libkrossapi.la \
    ../main/libkrossmain.la

INCLUDES = $(KROSS_INCLUDES) $(PYTHONINC) $(all_includes)
METASOURCES = AUTO
SUBDIRS = CXX scripts .
