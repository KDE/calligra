kde4_header()

add_subdirectory( parser )
add_subdirectory( drivers )

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )

ADD_DEFINITIONS( -D__KEXIDB__= -include ${CMAKE_SOURCE_DIR}/kexi/kexidb/global.h )

########### next target ###############

set(kexidb_LIB_SRCS 
   drivermanager.cpp 
   driver.cpp 
   driver_p.cpp 
   connection.cpp 
   keywords.cpp 
   object.cpp 
   field.cpp 
   utils.cpp 
   expression.cpp 
   connectiondata.cpp 
   fieldlist.cpp 
   tableschema.cpp 
   cursor.cpp 
   transaction.cpp 
   indexschema.cpp 
   queryschema.cpp 
   schemadata.cpp 
   global.cpp 
   relationship.cpp 
   roweditbuffer.cpp 
   msghandler.cpp 
   dbobjectnamevalidator.cpp 
   preparedstatement.cpp 
   dbproperties.cpp )

kde4_automoc(${kexidb_LIB_SRCS})

kde4_add_library(kexidb SHARED ${kexidb_LIB_SRCS})

target_link_libraries(kexidb  ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS} kexiutils )

set_target_properties(kexidb PROPERTIES VERSION 1.0.0 SOVERSION 1 )
install_targets(${LIB_INSTALL_DIR} kexidb )


########### install files ###############

install_files( /include/kexidb FILES  connection.h connectiondata.h cursor.h    driver.h drivermanager.h    error.h expression.h field.h fieldlist.h global.h object.h    schemadata.h tableschema.h queryschema.h indexschema.h    relationship.h parser/parser.h transaction.h preparedstatement.h    kexidb_export.h )
install_files( ${SERVICETYPES_INSTALL_DIR} FILES  kexidb_driver.desktop )

kde4_footer()



#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#lib_LTLIBRARIES = libkexidb.la
#
#INCLUDES = -I$(top_srcdir)/kexi $(all_includes)
#
#SUBDIRS = . parser drivers
#
#libkexidb_la_METASOURCES = AUTO
#
#libkexidb_la_SOURCES = drivermanager.cpp driver.cpp driver_p.cpp connection.cpp \
# keywords.cpp object.cpp field.cpp utils.cpp expression.cpp \
# connectiondata.cpp fieldlist.cpp tableschema.cpp cursor.cpp transaction.cpp  \
# indexschema.cpp queryschema.cpp schemadata.cpp global.cpp relationship.cpp \
# roweditbuffer.cpp msghandler.cpp \
# dbobjectnamevalidator.cpp preparedstatement.cpp \
# dbproperties.cpp
#
#noinst_HEADERS = drivermanager_p.h utils_p.h
#
#kexidbincludedir=$(includedir)/kexidb
#kexidbinclude_HEADERS=connection.h connectiondata.h cursor.h \
#  driver.h drivermanager.h \
#  error.h expression.h field.h fieldlist.h global.h object.h \
#  schemadata.h tableschema.h queryschema.h indexschema.h \
#  relationship.h parser/parser.h transaction.h preparedstatement.h \
#  kexidb_export.h
#
#libkexidb_la_LIBADD = $(LIB_QT) $(LIB_KDECORE) $(LIB_KIO) \
#	 $(top_builddir)/kexi/kexiutils/libkexiutils.la
#libkexidb_la_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#
#kde_servicetypes_DATA = kexidb_driver.desktop
#
#
#KDE_CXXFLAGS += -D__KEXIDB__= -include $(top_srcdir)/kexi/kexidb/global.h
#
