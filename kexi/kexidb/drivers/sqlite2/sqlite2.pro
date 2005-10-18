include( ../sqlite/sqlite_common.pro )

LIBS += $(KDEDIR)/lib/kexisql2$$KDELIBDEBUGLIB

INCLUDEPATH += $(KEXI)/3rdparty/kexisql/src ../sqlite/moc

system( bash kmoc ../sqlite )

DEFINES += SQLITE2

TARGET = kexidb_sqlite2driver$$KDELIBDEBUG

