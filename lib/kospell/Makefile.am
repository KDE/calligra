INCLUDES= $(all_includes)

lib_LTLIBRARIES  = libkospell.la

libkospell_la_SOURCES = koSconfig.cpp koSpelldlg.cpp koSpell.cpp koaspell.cpp koispell.cpp


libkospell_la_LDFLAGS = $(all_libraries) -version-info 5:0:1 -no-undefined
libkospell_la_LIBADD = $(LIB_KDEUI) $(LIBASPELL)

include_HEADERS = koSpell.h koSconfig.h koSpelldlg.h koaspell.h koispell.h
noinst_HEADERS = version.h

libkospell_la_METASOURCES = AUTO

check_PROGRAMS = kospelltest
kospelltest_SOURCES = koSpelltest.cpp applicationspelltest.cpp

kospelltest_LDADD = libkospell.la

