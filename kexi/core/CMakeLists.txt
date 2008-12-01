

include_directories( ${KOMAIN_INCLUDES} ${CMAKE_SOURCE_DIR}/libs ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES}  )

ADD_DEFINITIONS( 
-DKEXI_NO_CTXT_HELP=1
)

########### next target ###############

set(kexicore_LIB_SRCS 
   kexi_global.cpp 
   kexi.cpp 
   kexiaboutdata.cpp 
   KexiMainWindowIface.cpp
   KexiStandardAction.cpp
   kexidbconnectionset.cpp 
   kexiprojectset.cpp 
   kexiactionproxy.cpp 
   kexisharedactionhost.cpp 
   kexiactioncategories.cpp
   kexiproject.cpp 
   KexiWindow.cpp 
   KexiWindowData.cpp 
   KexiView.cpp 
   kexipartmanager.cpp 
   kexipartinfo.cpp 
   kexipartitem.cpp 
   kexipart.cpp 
   kexiprojectdata.cpp 
   kexiinternalpart.cpp 
   kexidragobjects.cpp 
 #  kexiuseraction.cpp 
 #  kexiuseractionmethod.cpp 
   kexistartupdata.cpp 
   kexiguimsghandler.cpp 
   kexitextmsghandler.cpp 
   kexidataiteminterface.cpp 
#unused   kexievents.cpp 
   kexidbshortcutfile.cpp 
   kexiblobbuffer.cpp 
   kexistaticpart.cpp
   kexitabledesignerinterface.cpp
   kexisearchandreplaceiface.cpp
   kexitemplateloader.cpp
)


kde4_add_library(kexicore SHARED ${kexicore_LIB_SRCS})

target_link_libraries(kexicore
  ${QT_QT3SUPPORT_LIBRARY}
  ${KDE4_KIO_LIBS}
  ${KDE4_KDEUI_LIBS}
  kexiutils
  kexidb
  ${KOPROPERTY_LIBS}
)

set_target_properties(kexicore PROPERTIES
    VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION}
    ${KDE4_DISABLE_PROPERTY_}LINK_INTERFACE_LIBRARIES "kexiutils;kexidb;${KOPROPERTY_LIBS}"
)
install(TARGETS kexicore  ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############






#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#lib_LTLIBRARIES = libkexicore.la
#libkexicore_la_SOURCES = kexi_global.cpp kexi.cpp \
#	keximainwindow.cpp \
#	kexidbconnectionset.cpp kexiprojectset.cpp \
#	kexiactionproxy.cpp kexisharedactionhost.cpp \
#	kexiproject.cpp kexidialogbase.cpp kexiviewbase.cpp \
#	kexipartmanager.cpp kexipartinfo.cpp kexipartitem.cpp kexipart.cpp \
#	kexiprojectdata.cpp kexiinternalpart.cpp \
#	kexidragobjects.cpp \
#	kexiuseraction.cpp kexiuseractionmethod.cpp \
#	kexistartupdata.cpp kexiguimsghandler.cpp kexitextmsghandler.cpp \
#	kexidataiteminterface.cpp kexievents.cpp \
#	kexidbshortcutfile.cpp \
#	kexiblobbuffer.cpp kexistaticpart.cpp
#
##kexipartdatasource.cpp
#
#libkexicore_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) $(VER_INFO) -Wnounresolved -no-undefined
#
#SUBDIRS = .
#
#libkexicore_la_LIBADD = $(LIB_KEXI_KMDI) \
#	$(top_builddir)/kexi/kexiutils/libkexiutils.la \
#	$(top_builddir)/kexi/kexidb/libkexidb.la \
#	$(top_builddir)/kexi/kexidb/parser/libkexidbparser.la \
#	$(top_builddir)/lib/koproperty/libkoproperty.la
#
#INCLUDES = $(LIB_KEXI_KMDI_INCLUDES) \
#	-I$(top_srcdir)/lib/komain \
#	-I$(top_srcdir)/lib \
#	-I$(top_srcdir)/kexi \
#	$(all_includes)
#
#noinst_HEADERS = kexiactionproxy_p.h kexicontexthelp_p.h kexisharedactionhost_p.h \
#	kexipartinfo_p.h
#
#METASOURCES = AUTO
