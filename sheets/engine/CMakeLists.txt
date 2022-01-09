
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${CMAKE_CURRENT_BINARY_DIR}/../
)

set (calligrasheetsengine_LIB_SRCS
    SheetsDebug.cpp
    Number.cpp
    Util.cpp
    CalculationSettings.cpp
    Localization.cpp

    Value.cpp
    ValueParser.cpp
    ValueConverter.cpp
    ValueCalc.cpp

    Function.cpp
    FunctionDescription.cpp

    Region.cpp
    RectStorage.cpp
    Damages.cpp

    CellBase.cpp
    MapBase.cpp
    SheetBase.cpp
    CellBaseStorage.cpp

    Binding.cpp
    BindingManager.cpp
    BindingModel.cpp
    BindingStorage.cpp
)

add_library(calligrasheetsengine SHARED ${calligrasheetsengine_LIB_SRCS})

generate_export_header(calligrasheetsengine
    EXPORT_FILE_NAME sheets_engine_generated_export.h
    BASE_NAME CALLIGRA_SHEETS_ENGINE
)
if (SHOULD_BUILD_DEVEL_HEADERS)

install( FILES
    sheets_engine_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/sheets_engine_generated_export.h

    calligra_sheets_limits.h

    CellBase.h
    CellBaseStorage.h
    MapBase.h
    Number.h
    PointStorage.h
    RectStorage.h
    Region.h
    RTree.h
    SheetBase.h
    Value.h
    ValueCalc.h
    ValueConverter.h
    ValueStorage.h
DESTINATION ${INCLUDE_INSTALL_DIR}/sheets COMPONENT Devel)

endif()

target_link_libraries(calligrasheetsengine
    PUBLIC
        Qt5::Core
        KF5::KDELibs4Support
)

set_target_properties(calligrasheetsengine PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS calligrasheetsengine ${INSTALL_TARGETS_DEFAULT_ARGS})

