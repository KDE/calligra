include_directories( ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )

########### next target ###############

set(krosskexidb_PART_SRCS 
   kexidbfield.cpp 
   kexidbfieldlist.cpp 
   kexidbschema.cpp 
   kexidbparser.cpp 
   kexidbcursor.cpp 
   kexidbconnectiondata.cpp 
   kexidbconnection.cpp 
   kexidbdriver.cpp 
   kexidbmodule.cpp )

kde4_automoc(${krosskexidb_PART_SRCS})

kde4_add_plugin(krosskexidb ${krosskexidb_PART_SRCS})



target_link_libraries(krosskexidb  ${KDE4_KDECORE_LIBS} kexidb kexidbparser )

install(TARGETS krosskexidb  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############
#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#INCLUDES = -I$(top_srcdir)/kexi $(KROSS_INCLUDES) $(all_includes)
#
#kde_module_LTLIBRARIES = krosskexidb.la
#
#krosskexidb_la_SOURCES = \
#    kexidbfield.cpp \
#    kexidbfieldlist.cpp \
#    kexidbschema.cpp \
#    kexidbparser.cpp \
#    kexidbcursor.cpp \
#    kexidbtransaction.cpp \
#    kexidbconnectiondata.cpp \
#    kexidbconnection.cpp \
#    kexidbdriver.cpp \
#    kexidbdrivermanager.cpp \
#    kexidbmodule.cpp
#
#krosskexidb_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
#krosskexidb_la_LIBADD = \
#    $(LIB_QT) \
#    $(LIB_KDECORE) \
#    $(LIB_KROSS_API) \
#    $(LIB_KROSS_MAIN) \
#    $(top_builddir)/kexi/kexidb/libkexidb.la \
#    $(top_builddir)/kexi/kexidb/parser/libkexidbparser.la
#
#METASOURCES = AUTO
#SUBDIRS = .
