include_directories(${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi/widget)

# the main plugin
set(kexi_csvimportexportplugin_SRCS
   KexiCsvImportExportPlugin.cpp
   KexiCSVImportDialogModel.cpp
   kexicsvimportdialog.cpp
   kexicsvimportoptionsdlg.cpp
   kexicsvwidgets.cpp
   kexicsvexportwizard.cpp
   kexicsvexport.cpp
)

add_library(kexi_csvimportexportplugin MODULE ${kexi_csvimportexportplugin_SRCS})
kcoreaddons_desktop_to_json(kexi_csvimportexportplugin kexi_csvimportexportplugin.desktop)

target_link_libraries(kexi_csvimportexportplugin
    PRIVATE
        kexicore
        kexiutils
        kexiextendedwidgets
        keximain

        KDb

        KF5::Codecs
        KF5::KIOWidgets #for KIO::pixmapForUrl...

        Qt5::Gui
)

if(MSVC)
   # avoid LNK1169 errors
   set_target_properties(kexi_csvimportexportplugin PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)
endif()

install(TARGETS kexi_csvimportexportplugin  DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
