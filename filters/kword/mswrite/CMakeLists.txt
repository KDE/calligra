
include_directories( ${KWMF_INCLUDES} )

########### next target ###############

set(mswriteimport_PART_SRCS 
   mswriteimport.cc 
   libmswrite_defs.cpp 
   structures_generated.cpp 
   structures_private.cpp 
   structures.cpp 
   libmswrite.cpp 
   ImportDialog.cc )

kde4_add_ui3_files(mswriteimport_PART_SRCS ImportDialogUI.ui )

kde4_add_plugin(mswriteimport WITH_PREFIX ${mswriteimport_PART_SRCS})



target_link_libraries(
    mswriteimport
    komain
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS mswriteimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(mswriteexport_PART_SRCS 
   mswriteexport.cc 
   libmswrite_defs.cpp 
   structures_generated.cpp 
   structures_private.cpp 
   structures.cpp 
   libmswrite.cpp )

kde4_add_plugin(mswriteexport WITH_PREFIX ${mswriteexport_PART_SRCS})



target_link_libraries(
    mswriteexport
    kowmf
    kwordexportfilters
    komain
    ${KDE4_KDECORE_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS mswriteexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kword_mswrite_import.desktop kword_mswrite_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})


