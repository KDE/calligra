KDE_CXXFLAGS=-UQT_NO_COMPAT
INCLUDES=-I$(srcdir)/.. $(all_includes)
check_PROGRAMS = richtext
richtext_SOURCES = main.cpp textedit.cpp
richtext_LDADD = ../libqt3stuff.la $(LIB_QT)
richtext_LDFLAGS = $(all_libraries)
METASOURCES = AUTO
KDE_OPTIONS = qtonly
