include_directories( ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/migration ${CMAKE_SOURCE_DIR}/kexi/kexiDB ${KDE4_INCLUDES}  )

########### next target ###############

set(kexihandler_csv_importexport_PART_SRCS 
   kexicsv_importexportpart.cpp 
   KexiCSVImportDialogModel.cpp
   kexicsvimportdialog.cpp 
   kexicsvimportoptionsdlg.cpp
   kexicsvwidgets.cpp 
   kexicsvexportwizard.cpp
   kexicsvexport.cpp
)

kde4_add_plugin(kexihandler_csv_importexport ${kexihandler_csv_importexport_PART_SRCS})

target_link_libraries(
    kexihandler_csv_importexport
    kexicore
    keximigrate
    kexidb
    ${CALLIGRADB_LIBS}
    kexiutils
    kexiextendedwidgets
    keximain
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KFILE_LIBS}
    ${QT_QTGUI_LIBRARY}
    )

if(MSVC)
   # avoid LNK1169 errors
   set_target_properties(kexihandler_csv_importexport PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)
endif()

install(TARGETS kexihandler_csv_importexport  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  kexicsv_importexporthandler.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
