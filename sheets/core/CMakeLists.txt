

set (odf_DIR_SRCS
    odf/SheetsOdfDoc.cpp
    odf/SheetsOdfMap.cpp
    odf/SheetsOdfSheet.cpp
    odf/SheetsOdfCell.cpp
    odf/SheetsOdfStyle.cpp
    odf/SheetsOdfRegion.cpp
    odf/SheetsOdfCondition.cpp
    odf/SheetsOdfValidity.cpp
    odf/GenValidationStyle.cpp
    )

set (ksp_DIR_SRCS
    ksp/SheetsKspCell.cpp
    ksp/SheetsKspConditions.cpp
    ksp/SheetsKspDoc.cpp
    ksp/SheetsKspMap.cpp
    ksp/SheetsKspSheet.cpp
    ksp/SheetsKspStyle.cpp
    ksp/SheetsKspValidity.cpp
)


set (calligrasheetscore_LIB_SRCS
    Currency.cpp
    Format.cpp
    ValueFormatter.cpp
    Cell.cpp
    Map.cpp
    Sheet.cpp
    SheetAccessModel.cpp
    Style.cpp
    StyleStorage.cpp
    StyleManager.cpp
    CellStorage.cpp
    SheetModel.cpp
    Condition.cpp
    ConditionsStorage.cpp
    RowFormatStorage.cpp
    ColFormatStorage.cpp
    HeaderFooter.cpp
    Database.cpp
    DatabaseStorage.cpp
    DataFilter.cpp
    Binding.cpp
    BindingManager.cpp
    BindingModel.cpp
    BindingStorage.cpp
    PrintSettings.cpp
    DocBase.cpp
    ShapeApplicationData.cpp

    ${odf_DIR_SRCS}
    ${ksp_DIR_SRCS}
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
    RowFormatStorage.h
    Database.h
    DataFilter.h
    DocBase.h
    PrintSettings.h
    odf/OdfLoadingContext.h

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

