include( ../common.pro )

win32:QMAKE_CXXFLAGS += /I$(MYSQL_INC)

win32:LIBS += $(MYSQL_LIB)/mysqlclient.lib

TARGET		= kexidb_mysqldriver$$KDELIBDEBUG

system( bash kmoc )

SOURCES = \
mysqlconnection_p.cpp \
mysqlconnection.cpp \
mysqldriver.cpp \
mysqlcursor.cpp \
mysqlkeywords.cpp

HEADERS		= 

