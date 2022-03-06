
### Shared library with part functionality

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


set (calligrasheetsprivate_LIB_SRCS
    CanvasBase.cpp
    Canvas.cpp
    CanvasItem.cpp
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
    ToolRegistry.cpp
    TabBar.cpp
    View.cpp
    commands/DefinePrintRangeCommand.cpp
    commands/PageLayoutCommand.cpp

    ${dialogs_DIR_SRCS}
)


add_library(calligrasheetsprivate SHARED ${calligrasheetsprivate_LIB_SRCS})

target_include_directories( calligrasheetsprivate
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_BINARY_DIR}/../
)


generate_export_header(calligrasheetsprivate
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

DESTINATION ${INCLUDE_INSTALL_DIR}/sheets/part COMPONENT Devel)

endif()

target_link_libraries(calligrasheetsprivate
    PUBLIC
        calligrasheetsengine
        calligrasheetscore
        calligrasheetsui
        komain
)

set_target_properties(calligrasheetsprivate PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS calligrasheetsprivate ${INSTALL_TARGETS_DEFAULT_ARGS})


### The actual part

set (calligrasheetspart_PART_SRCS Factory_init.cpp )

add_library(calligrasheetspart MODULE ${calligrasheetspart_PART_SRCS})

calligra_part_desktop_to_json(calligrasheetspart sheetspart.desktop)

target_link_libraries(calligrasheetspart 
    PUBLIC
        calligrasheetsengine
        calligrasheetscore
        calligrasheetsui
        calligrasheetsprivate
)

install(TARGETS calligrasheetspart  DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/parts)

install( FILES  calligrasheets.rc calligrasheets_readonly.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/calligrasheets)
install( FILES  calligrasheets.kcfg DESTINATION ${KCFG_INSTALL_DIR} )
install( FILES  calligrasheetsrc DESTINATION ${CONFIG_INSTALL_DIR})


