

set (calligrasheetsengine_LIB_SRCS
    SheetsDebug.cpp
    CS_Time.cpp
    Format.cpp
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
    FunctionRepository.cpp
    FunctionModule.cpp
    FunctionModuleRegistry.cpp

    Region.cpp
    RectStorage.cpp
    Damages.cpp
    ProtectableObject.cpp

    CellBase.cpp
    MapBase.cpp
    SheetBase.cpp
    CellBaseStorage.cpp

    RecalcManager.cpp
    DependencyManager.cpp
    NamedAreaManager.cpp
    Validity.cpp
    ValidityStorage.cpp
    Formula.cpp

)

add_library(calligrasheetsengine SHARED ${calligrasheetsengine_LIB_SRCS})

target_include_directories( calligrasheetsengine
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

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
    ProtectableObject.h
    RectStorage.h
    Region.h
    RTree.h
    SheetBase.h
    Value.h
    ValueCalc.h
    ValueConverter.h
    ValueStorage.h
DESTINATION ${KDE_INSTALL_INCLUDEDIR}/sheets COMPONENT Devel)

endif()

target_link_libraries(calligrasheetsengine
    PUBLIC
        Qt6::Core
        Qt6::Xml
        KF6::CoreAddons
        KF6::ConfigCore
        KF6::I18n
    PRIVATE
        koplugin
)

set_target_properties(calligrasheetsengine PROPERTIES
    VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)
install(TARGETS calligrasheetsengine ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

