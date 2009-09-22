project(kexi)

SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

IF(EXISTS ${CMAKE_SOURCE_DIR}/kexi/CMakeSettings.txt)
  INCLUDE(${CMAKE_SOURCE_DIR}/kexi/CMakeSettings.txt)
ENDIF(EXISTS ${CMAKE_SOURCE_DIR}/kexi/CMakeSettings.txt)

ADD_DEFINITIONS(
  -DKEXI_NO_CURSOR_PROPERTY 
  -DKEXI_NO_CTXT_HELP
  -DKEXI_NO_SUBFORM
  -DKEXI_DB_COMBOBOX_WIDGET
  -DDB_TEMPLATES
  -DKEXI_NO_UNFINISHED
  -DKEXI_SCRIPTS_SUPPORT
)

#no default: add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44010)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-kexi.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kexi.h )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${KOMAIN_INCLUDES}
  ${KDE4_INCLUDES}
  ${QT_INCLUDES}
  ${CMAKE_SOURCE_DIR}/libs
)

add_definitions(-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

SET(KOPROPERTY_TARGETNAME koproperty)                           # TODO remove when we move to independent place
SET(KOPROPERTY_LIBS ${KOPROPERTY_TARGETNAME})                   # TODO remove when we move to independent place
SET(KOPROPERTY_LIB_VERSION ${GENERIC_KOFFICE_LIB_VERSION})      # TODO remove when we move to independent place
SET(KOPROPERTY_LIB_SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION})  # TODO remove when we move to independent place
SET(KOPROPERTY_USE_KOLIBS TRUE)                                 # TODO remove when we move to independent place
add_subdirectory( koproperty )

add_subdirectory( 3rdparty )
add_subdirectory( kexiutils )
add_subdirectory( kexidb )
add_subdirectory( core )
add_subdirectory( widget )
add_subdirectory( formeditor )

if(WITH_WEBFORMS)
  macro_optional_find_package(PionNet)
  macro_log_feature(PionNet_FOUND "Pion Network Library" "C++ development library for implementing lightweight HTTP interfaces" "http://www.pion.org/projects/pion-network-library" FALSE "" "Required by Kexi Web Forms")

  macro_optional_find_package(GoogleCtemplate)
  macro_log_feature(GOOGLE_CTEMPLATE_FOUND "Google Ctemplate" "Simple but powerful template language for C++" "http://code.google.com/p/google-ctemplate/" FALSE "" "Required by Kexi Web Forms")
  
  macro_log_feature(Boost_FOUND "Boost" "Boost C++ Libraries" "http://www.boost.org" FALSE "" "Required by Kexi Web Forms")

  if(GOOGLE_CTEMPLATE_FOUND AND Boost_FOUND AND PionNet_FOUND)
    set(WITH_WEBFORMS TRUE CACHE STRING "Build with Kexi Web Forms support")
    add_definitions(-DKEXIWEBFORMS_SUPPORTED)
    message(STATUS "Kexi Web Forms support enabled.")
    macro_optional_add_subdirectory(webforms)
  else(GOOGLE_CTEMPLATE_FOUND AND Boost_FOUND AND PionNet_FOUND)
    set(WITH_WEBFORMS FALSE CACHE STRING "Build with Kexi Web Forms support")
    message(STATUS "Kexi Web Forms support DISABLED, one of the following libraries is missing:")
    message(STATUS "- Google CTemplate")
    message(STATUS "- Boost")
    message(STATUS "- Pion Network Library")
  endif(GOOGLE_CTEMPLATE_FOUND AND Boost_FOUND AND PionNet_FOUND)
else(WITH_WEBFORMS)
  set(WITH_WEBFORMS FALSE CACHE STRING "Build with Kexi Web Forms support")
endif(WITH_WEBFORMS)

add_subdirectory( data )
add_subdirectory( pics )

add_subdirectory( main )

add_subdirectory( migration )
add_subdirectory( plugins )
add_subdirectory( tests )

add_subdirectory(shapes)

########### next target ###############

set(kexi_SRCS main.cpp )
kde4_add_app_icon(kexi_SRCS "${KDE4_ICON_DIR}/oxygen/*/apps/kexi.png")
kde4_add_executable(kexi ${kexi_SRCS})
target_link_libraries(kexi 
  ${KDE4_KFILE_LIBS} ${QT_QT3SUPPORT_LIBRARY}
  kexidb
  keximain
)
install(TARGETS kexi ${INSTALL_TARGETS_DEFAULT_ARGS})

# that functionality got moved to kexi/main2/CMakeLists.txt
#set(kexi_KDEINIT_SRCS main.cpp )
#kde4_add_kdeinit_executable( kexi ${kexi_KDEINIT_SRCS})
#target_link_libraries(kdeinit_kexi  ${KDE4_KDECORE_LIBS} kexicore keximain )
#install(TARGETS kdeinit_kexi  ${INSTALL_TARGETS_DEFAULT_ARGS})
#target_link_libraries( kexi kdeinit_kexi )
#install(TARGETS kexi ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

# the old KDE3-way was to install the desktop-file here while the new KDE4 way is to install to ${XDG_APPS_INSTALL_DIR}
#install( FILES  kexi.desktop  DESTINATION ${APPLNK_INSTALL_DIR}/Office)

#install(FILES kexi.rc kexi_readonly.rc DESTINATION ${DATA_INSTALL_DIR}/kexi)
#install(FILES kexipart.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES kexi.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
#install(FILES kexi.kcfg DESTINATION  ${KCFG_INSTALL_DIR} )

#kde4_create_doxygen_docs( EXCLUDE 3rdparty kexidb/parser/sqlparser.h kexidb/drivers/odbc  )

########### OLD ###############
#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#lib_LTLIBRARIES =
#kdeinit_LTLIBRARIES = kexi.la
#bin_PROGRAMS =
#kexi_la_SOURCES = main.cpp
#kexi_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) $(VER_INFO)
#kexi_la_LIBADD = ./core/libkexicore.la ./main/libkeximain.la
#
#SUBDIRS= 3rdparty kexiutils kexidb core widget formeditor \
#data pics main migration plugins . tests
#
##(will be reenabled after main/startup move)
#
#kdelnkdir = $(kde_appsdir)/Office
#kdelnk_DATA = kexi.desktop
#
#install-data-local:
##	$(mkinstalldirs) $(kde_servicetypesdir)/
##	$(INSTALL_DATA) $(srcdir)/kexiinterface.desktop $(kde_servicetypesdir)/kexiinterface.desktop
##	$(mkinstalldirs) $(kde_servicedir)
##	$(INSTALL_DATA) $(srcdir)/interfaces/mysqlinterface.desktop $(kde_servicesdir)/mysqlinterface.desktop
#
## kde_appsdir         Where your application's menu entry (.desktop) should go to.
## kde_icondir         Where your icon should go to - better use KDE_ICON.
## kde_sounddir        Where your sounds should go to.
## kde_htmldir         Where your docs should go to. (contains lang subdirs)
## kde_datadir         Where you install application data. (Use a subdir)
## kde_locale          Where translation files should go to. (contains lang subdirs)
## kde_cgidir          Where cgi-bin executables should go to.
## kde_confdir         Where config files should go to (system-wide ones with default values).
## kde_mimedir         Where mimetypes .desktop files should go to.
## kde_servicesdir     Where services .desktop files should go to.
## kde_servicetypesdir Where servicetypes .desktop files should go to.
## kde_toolbardir      Where general toolbar icons should go to (deprecated, use KDE_ICON).
## kde_wallpaperdir    Where general wallpapers should go to.
## kde_templatesdir    Where templates for the "New" menu (Konqueror/KDesktop) should go to.
## kde_bindir          Where executables should go to. Use bin_PROGRAMS or bin_SCRIPTS.
## kde_libdir          Where shared libraries should go to. Use lib_LTLIBRARIES.
## kde_moduledir       Where modules (e.g. parts) should go to. Use kde_module_LTLIBRARIES.
## kde_styledir        Where Qt/KDE widget styles should go to (new in KDE 3).
## kde_designerdir     Where Qt Designer plugins should go to (new in KDE 3).
#
## set the include path for X, qt and KDE
#INCLUDES = $(LIB_KEXI_KMDI_INCLUDES) $(all_includes)
#
#METASOURCES = AUTO
#
## Note: koffice/kexi/formeditor should be kept out of kexi.pot
## as it might get moved out of Kexi.
##
## If a second directory must be "pruned", the syntax becomes:
## find . \( -name formeditor -o -name test \) -prune , -name \*.ui
## (The comma is needed or the pruned directory name would be printed to stdout)
#messages: rc.cpp
#	rm -f tips.cpp
##	(cd data && $(PREPARETIPS) > ../tips.cpp)
#	EXCLUDE="-path ./formeditor -o -path ./doc \
#	-o -path ./plugins/importwizard -o -path ./tests \
#	-o -path ./scriptingplugins -o -path ./plugins/scripting -o -path ./3rdparty"; \
#	LIST="data/*.rc `find . \( \( $$EXCLUDE \) -prune -o -name \*.ui \) -type f | grep -v -e '/\.'`"; \
#	if test -n "$$LIST"; then \
#	 $(EXTRACTRC) $$LIST >> rc.cpp; \
#	fi; \
#	LIST=`find . \( \( $$EXCLUDE \) -prune -o -name \*.h -o -name \*.cpp \) -type f | grep -v -e '/\.' -e kexidswelcome.cpp`; \
#	if test -n "$$LIST"; then \
#	 $(XGETTEXT) $$LIST -o $(podir)/kexi.pot; \
#	fi
#	rm -f tips.cpp
#
#DOXYGEN_EXCLUDE = 3rdparty kexidb/parser/sqlparser.h kexidb/drivers/odbc
#include $(top_srcdir)/admin/Doxyfile.am
