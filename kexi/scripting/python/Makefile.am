include $(top_srcdir)/kexi/Makefile.global

INCLUDES = -I$(top_srcdir)/kexi $(PYTHONINC) $(all_includes)

kde_module_LTLIBRARIES = krosspython.la

krosspython_la_LDFLAGS = $(LIBPYTHON) $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module

krosspython_la_SOURCES = \
    pythonextension.cpp \
    pythonmodule.cpp \
    pythonobject.cpp \
    pythonscript.cpp \
    pythonsecurity.cpp \
    pythoninterpreter.cpp

krosspython_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    CXX/libkrosspythoncxx.la \
    ../api/libkrossapi.la \
    ../main/libkrossmain.la

METASOURCES = AUTO
SUBDIRS = CXX .
#TODO install-/modulpath needs to be a runtime-option!
#modules
