include( sqlite_common.pro )

win32:LIBS += $(KDEDIR)/lib/kexisql3.lib

INCLUDEPATH += $(KEXI)/3rdparty/kexisql3/src

TARGET		= kexidb_sqlite3driver$$KDELIBDEBUG

