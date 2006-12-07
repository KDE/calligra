

add_subdirectory( startup )
add_subdirectory( printing )

include_directories( ${CMAKE_SOURCE_DIR}/lib/kofficeui ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi/main ${CMAKE_SOURCE_DIR}/kexi/main/startup ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/migration ${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/lib/kofficecore ${KDE4_INCLUDES}  )


########### next target ###############

set(keximain_LIB_SRCS 
   keximainwindowimpl.cpp 
   kexistatusbar.cpp 
   kexinamewidget.cpp 
   kexinamedialog.cpp 
   kexinewstuff.cpp )

kde4_automoc(${keximain_LIB_SRCS})

kde4_add_library(keximain SHARED ${keximain_LIB_SRCS})

target_link_libraries(keximain  ${KDE4_KDECORE_LIBS} kexidb kexicore kexiutils keximainstartup keximainprinting koproperty kexiextendedwidgets ${KDE4_KNEWSTUFF_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KDEPRINT_LIBS} )

set_target_properties(keximain PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS keximain  DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############






#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#lib_LTLIBRARIES = libkeximain.la
#libkeximain_la_SOURCES = \
#	keximainwindowimpl.cpp \
#	kexibrowser.cpp kexibrowseritem.cpp \
#	kexistatusbar.cpp \
#	kexinamewidget.cpp kexinamedialog.cpp \
#	kexinewstuff.cpp
#
#libkeximain_la_LDFLAGS = -no-undefined $(KDE_RPATH) $(all_libraries) \
#	$(VER_INFO) -Wnounresolved
#
#SUBDIRS = startup printing .
#
#libkeximain_la_LIBADD =  $(top_builddir)/kexi/kexidb/libkexidb.la $(top_builddir)/kexi/core/libkexicore.la \
#	$(top_builddir)/kexi/kexiutils/libkexiutils.la \
#	$(top_builddir)/kexi/main/startup/libkeximainstartup.la \
#	$(top_builddir)/kexi/main/printing/libkeximainprinting.la \
#	$(top_builddir)/lib/koproperty/libkoproperty.la \
#	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
#	$(LIB_KFEEDBACK) $(LIB_KEXI_KMDI) $(LIB_KNEWSTUFF) $(LIB_KIO) $(LIB_KDEPRINT)
#
##disabled	../migration/libkeximigrate.la
#
#INCLUDES = $(INC_KFEEDBACK) $(LIB_KEXI_KMDI_INCLUDES) \
#	-I$(top_builddir)/lib/kofficeui -I$(top_srcdir)/lib/kofficeui \
#	-I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/core \
#	-I$(top_srcdir)/kexi/main \
#	-I$(top_srcdir)/kexi/main/startup -I$(top_srcdir)/kexi/widget \
#	-I$(top_srcdir)/kexi/migration -I$(top_srcdir)/lib -I$(top_srcdir)/lib/kofficecore \
#	$(all_includes)
#
#noinst_HEADERS = ksplitter.h kexibrowser_p.h keximainwindowimpl_p.h
#
#METASOURCES = AUTO
