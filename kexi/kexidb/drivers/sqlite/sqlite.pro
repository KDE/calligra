include( ../common.pro )

DEFINES += MAKE_KEXIDB_SQLITE_DRIVER_LIB

win32:LIBS += $(QKW)/3rdparty/sqlite/lib/sqlite.lib

win32:QMAKE_CXXFLAGS += /I$(QKW)/3rdparty/sqlite/include

TARGET		= kexidb_sqlitedriver$$KDEBUG

system( moc sqlitedriver.h -o moc/sqlitedriver.moc )
system( moc sqliteconnection.h -o moc/sqliteconnection.moc )

#system( moc mysqlrecord.h -o moc/mysqlrecord.moc )

SOURCES = \
sqliteconnection.cpp \
sqlitedriver.cpp \
sqlitecursor.cpp

#mysqlrecord.cpp \
#mysqlresult.cpp

HEADERS		= 

