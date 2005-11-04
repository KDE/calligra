include( sqlite_common.pro )

win32:LIBS += $(KDEDIR)/lib/kexisql3$$KDELIBDEBUGLIB

INCLUDEPATH += $(KEXI)/3rdparty/kexisql3/src

TARGET		= kexidb_sqlite3driver$$KDELIBDEBUG

SOURCES += sqlitekeywords.cpp
