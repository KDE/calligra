
set (calligrasheetscore_LIB_SRCS
    Currency.cpp
    Format.cpp
    ValueFormatter.cpp
    Cell.cpp
    Map.cpp
    Sheet.cpp
    Style.cpp
    StyleStorage.cpp
    CellStorage.cpp
    Condition.cpp
    ConditionsStorage.cpp
    RowFormatStorage.cpp
    RowColumnFormat.cpp
    Database.cpp
    DatabaseStorage.cpp
    DataFilter.cpp
)

add_library(calligrasheetscore SHARED ${calligrasheetscore_LIB_SRCS})

target_include_directories( calligrasheetscore
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_BINARY_DIR}/../
)


generate_export_header(calligrasheetscore
    EXPORT_FILE_NAME sheets_core_generated_export.h
    BASE_NAME CALLIGRA_SHEETS_CORE
)
if (SHOULD_BUILD_DEVEL_HEADERS)

install( FILES
    sheets_core_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/sheets_core_generated_export.h

    Condition.h
    Currency.h
    Format.h
    Cell.h
    Map.h
    Sheet.h
    Style.h
    CellStorage.h
    RowColumnFormat.h
    Database.h
    DataFilter.h
DESTINATION ${INCLUDE_INSTALL_DIR}/sheets COMPONENT Devel)

endif()

target_link_libraries(calligrasheetscore
    PUBLIC
        calligrasheetsengine
        komain
)

set_target_properties(calligrasheetscore PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS calligrasheetscore ${INSTALL_TARGETS_DEFAULT_ARGS})

