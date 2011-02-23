include_directories(${CMAKE_SOURCE_DIR}/kspread ${KOMAIN_INCLUDES})

set(kspread_plugin_googlespreadsheets_SRCS 
    GoogleSpreadsheetsPlugin.cpp
    LoginDialog.cpp
    ServiceManager.cpp
    DocumentsListService.cpp
    DocumentsListDialog.cpp
    DocumentsListTreeItem.cpp
    DocumentsListTreeModel.cpp
    SpreadsheetService.cpp
    #Spreadsheet.cpp
    #Worksheet.cpp
)

kde4_add_ui_files(kspread_plugin_googlespreadsheets_SRCS
    LoginDialog.ui
    DocumentsListDialog.ui
)

kde4_add_plugin(kspread_plugin_googlespreadsheets ${kspread_plugin_googlespreadsheets_SRCS})

target_link_libraries(kspread_plugin_googlespreadsheets kspreadcommon komain ${KDE4_KPARTS_LIBS} ${QT_LIBRARIES})

install(TARGETS kspread_plugin_googlespreadsheets DESTINATION ${PLUGIN_INSTALL_DIR})
#install(FILES googlespreadsheets.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES googlespreadsheetsplugin.rc DESTINATION ${DATA_INSTALL_DIR}/kspread/kpartplugins)
