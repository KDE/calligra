include( ../common.pro )

DEFINES += MAKE_KEXIDB_SQLITE_DRIVER_LIB

win32:LIBS += $(KDEDIR)/lib/kexisql.lib

win32:QMAKE_CXXFLAGS += /I$(KEXI)/3rdparty/kexisql/src

TARGET		= kexidb_sqlitedriver$$KDEBUG

system( moc sqlitedriver.h -o moc/sqlitedriver.moc )
system( moc sqliteconnection.h -o moc/sqliteconnection.moc )

SOURCES = \
sqliteconnection.cpp \
sqlitedriver.cpp \
sqlitecursor.cpp

HEADERS		= 

