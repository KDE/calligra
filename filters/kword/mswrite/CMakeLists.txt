


include_directories(${CMAKE_SOURCE_DIR}/libs/kwmf )

########### next target ###############

set(mswriteimport_PART_SRCS 
   mswriteimport.cc 
   libmswrite_defs.cpp 
   structures_generated.cpp 
   structures_private.cpp 
   structures.cpp 
   libmswrite.cpp 
   ImportDialog.cc )

kde4_automoc(${mswriteimport_PART_SRCS})

kde4_add_ui3_files(mswriteimport_PART_SRCS ImportDialogUI.ui )

kde4_add_plugin(mswriteimport WITH_PREFIX ${mswriteimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} mswriteimport )

target_link_libraries(mswriteimport kofficecore)

install(TARGETS mswriteimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(mswriteexport_PART_SRCS 
   mswriteexport.cc 
   libmswrite_defs.cpp 
   structures_generated.cpp 
   structures_private.cpp 
   structures.cpp 
   libmswrite.cpp )

kde4_automoc(${mswriteexport_PART_SRCS})

kde4_add_plugin(mswriteexport WITH_PREFIX ${mswriteexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} mswriteexport )

target_link_libraries(mswriteexport  ${KDE4_KDECORE_LIBS} kowmf kwordexportfilters )

install(TARGETS mswriteexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_mswrite_import.desktop kword_mswrite_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


