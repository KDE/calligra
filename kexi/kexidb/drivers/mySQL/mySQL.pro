include( ../common.pro )

DEFINES += MAKE_KEXIDB_MYSQL_DRIVER_LIB

win32:QMAKE_CXXFLAGS += /I$(MYSQL_INC)

win32:LIBS += $(MYSQL_LIB)/mysqlclient.lib

TARGET		= kexidb_mysqldriver$$KDEBUG

system( bash kmoc )

SOURCES = \
mysqlconnection.cpp \
mysqldriver.cpp \
mysqlcursor.cpp \
mysqlkeywords.cpp

HEADERS		= 

