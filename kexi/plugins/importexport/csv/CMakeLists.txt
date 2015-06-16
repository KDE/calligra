include_directories( ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/migration)

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

add_library(kexihandler_csv_importexport MODULE ${kexihandler_csv_importexport_PART_SRCS})

target_link_libraries(
    kexihandler_csv_importexport
    kexicore
    keximigrate
    kexiutils
    kexiextendedwidgets
    keximain

    KDb

    Qt5::Gui
)

if(MSVC)
   # avoid LNK1169 errors
   set_target_properties(kexihandler_csv_importexport PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)
endif()

install(TARGETS kexihandler_csv_importexport  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(TARGETS kexi_csvimportexportplugin  DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
