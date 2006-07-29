include( ../common.pro )

INCLUDEPATH += $(MYSQL_INC) $(MYSQL_INC)/mysql

contains(CONFIG,debug) {
	win32:LIBS += $(MYSQL_LIB)/debug/libmysql.lib
	win32:QMAKE_LFLAGS += /NODEFAULTLIB:LIBCMTD.LIB
}
!contains(CONFIG,debug) {
#	win32:LIBS += $(MYSQL_LIB)/opt/mysqlclient.lib
	win32:LIBS += $(MYSQL_LIB)/opt/libmysql.lib
#	win32:QMAKE_LFLAGS += /NODEFAULTLIB:MSVCRT.LIB
}

TARGET = kexidb_mysqldriver$$KDELIBDEBUG

system( bash kmoc )

SOURCES = \
mysqlconnection_p.cpp \
mysqlconnection.cpp \
mysqldriver.cpp \
mysqlcursor.cpp \
mysqlkeywords.cpp \
mysqlpreparedstatement.cpp

HEADERS		= 

