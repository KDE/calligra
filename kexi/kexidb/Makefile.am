include $(top_srcdir)/kexi/Makefile.global

lib_LTLIBRARIES = libkexidb.la

INCLUDES = -I$(top_srcdir)/kexi $(all_includes)

SUBDIRS = . parser drivers

libkexidb_la_METASOURCES = AUTO

libkexidb_la_SOURCES = drivermanager.cpp driver.cpp driver_p.cpp connection.cpp \
 keywords.cpp object.cpp field.cpp utils.cpp expression.cpp \
 connectiondata.cpp fieldlist.cpp tableschema.cpp cursor.cpp transaction.cpp  \
 indexschema.cpp queryschemaparameter.cpp queryschema.cpp schemadata.cpp global.cpp \
 relationship.cpp roweditbuffer.cpp msghandler.cpp \
 dbobjectnamevalidator.cpp fieldvalidator.cpp preparedstatement.cpp \
 dbproperties.cpp admin.cpp alter.cpp lookupfieldschema.cpp simplecommandlineapp.cpp

noinst_HEADERS = drivermanager_p.h utils_p.h

kexidbincludedir=$(includedir)/kexidb
kexidbinclude_HEADERS= \
admin.h \
alter.h \
connectiondata.h \
connection.h \
cursor.h \
dbobjectnamevalidator.h \
dbproperties.h \
driver.h \
drivermanager.h \
driver_p.h \
error.h \
expression.h \
field.h \
fieldlist.h \
fieldvalidator.h \
global.h \
indexschema.h \
kexidb_export.h \
lookupfieldschema.h \
msghandler.h \
object.h \
preparedstatement.h \
queryschema.h \
queryschemaparameter.h \
relationship.h \
roweditbuffer.h \
schemadata.h \
simplecommandlineapp.h \
tableschema.h \
transaction.h \
utils.h \
parser/parser.h

libkexidb_la_LIBADD = $(LIB_QT) $(LIB_KDECORE) $(LIB_KIO) \
	 $(top_builddir)/kexi/kexiutils/libkexiutils.la
libkexidb_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) $(VER_INFO)

kde_servicetypes_DATA = kexidb_driver.desktop


KDE_CXXFLAGS += -D__KEXIDB__= -include $(top_srcdir)/kexi/kexidb/global.h

