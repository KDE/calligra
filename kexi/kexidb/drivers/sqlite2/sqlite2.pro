include( ../sqlite/sqlite_common.pro )

win32:LIBS += $(KDEDIR)/lib/kexisql2.lib

win32:INCLUDEPATH += $(KEXI)/3rdparty/kexisql/src ../sqlite/moc

system( bash kmoc ../sqlite )

TARGET = kexidb_sqlite2driver$$KDELIBDEBUG

