
INCLUDES = $(KOFFICE_INCLUDES) $(all_includes) $(MICO_INCLUDES) -I../kdb
#LDFLAGS = -g $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDFLAGS = -g $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH) -lpq++
LDADD = ../../lib/kofficeui/libkofficeui.la ../kdb/libkdb.la

####### Files
 
bin_PROGRAMS = ktables

ktables_SOURCES = tableselectdata.cpp tableselect.cpp ktablesserver.cpp \
	ktablesview.cpp ktablesdoc.cpp ktables.cpp main.cpp 

noinst_HEADERS = tableselectdata.h tableselect.h ktablesserver.h \
	ktablesview.h ktablesdoc.h ktables.h

METASOURCES = tableselect.moc ktablesserver.moc \
	ktablesdoc.moc ktablesview.moc ktables.moc

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(ktables_SOURCES) -o ../po/ktables.pot

