kde_module_LTLIBRARIES = keximigrate_mdb.la
kde_services_DATA = keximigrate_mdb.desktop

keximigrate_mdb_la_SOURCES = mdbmigrate.cpp
AM_CPPFLAGS = \
  -DMDB_NO_BACKENDS=1 -DMDB_NO_STATS=1 \
  -I$(top_srcdir)/keximdb/src/mdbtools/include \
  -I$(KEXIDB_INC) -I$(KEXIDB_INC)/kexidb \
  $(GLIB_CFLAGS) $(all_includes)

keximigrate_mdb_la_METASOURCES = AUTO

keximigrate_mdb_la_LIBADD = \
  $(LIB_QT) $(GLIB_LIBADD) -L$(KEXIDB_LIB) -lkeximigrate \
  ../mdbtools/libmdb/libmdb.la

keximigrate_mdb_la_LDFLAGS = \
  $(all_libraries) $(GLIB_LDFLAGS) \
  -module $(KDE_PLUGIN) -no-undefined

noinst_HEADERS =  mdbmigrate.h

