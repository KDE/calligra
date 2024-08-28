
### Shared library with part functionality

set (chart_DIR_SRCS
    chart/ChartDatabaseSelectorFactory.cpp
    chart/ChartDatabaseSelector.cpp
    chart/ChartDialog.cpp
    )

ki18n_wrap_ui(chart_DIR_SRCS
    chart/ChartDatabaseSelector.ui
    )


set (dialogs_DIR_SRCS
    dialogs/PageLayoutDialog.cpp
    dialogs/PreferenceDialog.cpp
    dialogs/SheetPropertiesDialog.cpp
    dialogs/SheetSelectPage.cpp
    )

ki18n_wrap_ui(dialogs_DIR_SRCS
    dialogs/FileOptionsWidget.ui
    dialogs/InterfaceOptionsWidget.ui
    dialogs/PageLayoutSheetPage.ui
    dialogs/SheetPropertiesWidget.ui
    dialogs/SheetSelectWidget.ui
)

if(UNIX AND NOT APPLE AND NOT ANDROID AND NOT CALLIGRA_FLATPAK)
set (interfaces_DIR_SRCS
    MapAdaptor.cpp
    SheetAdaptor.cpp
    ViewAdaptor.cpp
    )
endif()

set (calligrasheetspartlib_LIB_SRCS
    CanvasBase.cpp
    Canvas.cpp
    CanvasItem.cpp
    CellEditorWidget.cpp
    CellTool.cpp
    CellToolFactory.cpp
    Doc.cpp
    Part.cpp
    Factory.cpp
    Find.cpp
    Headers.cpp
    HeaderWidgets.cpp
    HeaderItems.cpp
    PrintJob.cpp
    RightToLeftPaintingStrategy.cpp
    ToolRegistry.cpp
    TabBar.cpp
    View.cpp
    commands/DefinePrintRangeCommand.cpp
    commands/PageLayoutCommand.cpp

    ${chart_DIR_SRCS}
    ${dialogs_DIR_SRCS}
)

if(UNIX AND NOT APPLE AND NOT ANDROID AND NOT CALLIGRA_FLATPAK)
set (calligrasheetspartlib_LIB_SRCS
    ${calligrasheetspartlib_LIB_SRCS}
    ${interfaces_DIR_SRCS}
    )
endif()



add_library(calligrasheetspartlib SHARED ${calligrasheetspartlib_LIB_SRCS})

target_include_directories( calligrasheetspartlib
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_BINARY_DIR}/../
)


generate_export_header(calligrasheetspartlib
    EXPORT_FILE_NAME sheets_part_generated_export.h
    BASE_NAME CALLIGRA_SHEETS_PART
)
if (SHOULD_BUILD_DEVEL_HEADERS)

install( FILES
    sheets_part_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/sheets_part_generated_export.h

    CanvasBase.h
    CanvasItem.h
    CellTool.h
    Doc.h
    Part.h
    Find.h
    HeaderItems.h
    Headers.h
    ToolRegistry.h
    View.h

DESTINATION ${KDE_INSTALL_INCLUDEDIR}/sheets/part COMPONENT Devel)

endif()

target_link_libraries(calligrasheetspartlib
    PUBLIC
        calligrasheetsengine
        calligrasheetscore
        calligrasheetsui
        komain
        koplugin
        KF6::KCMUtils
        KF6::NotifyConfig
)

set_target_properties(calligrasheetspartlib PROPERTIES
    VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)
install(TARGETS calligrasheetspartlib ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


### The actual part

set (calligrasheetspart_PART_SRCS Factory_init.cpp )

add_library(calligrasheetspart MODULE ${calligrasheetspart_PART_SRCS})


target_link_libraries(calligrasheetspart 
    PUBLIC
        calligrasheetsengine
        calligrasheetscore
        calligrasheetsui
        calligrasheetspartlib
)

install(TARGETS calligrasheetspart  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/parts)

install( FILES  calligrasheets.rc calligrasheets_readonly.rc DESTINATION ${KDE_INSTALL_KXMLGUIDIR}/calligrasheets)
install( FILES  calligrasheets.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR} )
install( FILES  calligrasheetsrc DESTINATION ${KDE_INSTALL_CONFDIR})


