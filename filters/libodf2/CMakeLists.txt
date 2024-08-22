#add_subdirectory( tests )

include_directories(${KOODF_INCLUDES}
                    ${KOODF2_INCLUDES}

                    # FIXME: This should not be here. Make sure that the chart
                    #        classes parse the number formats while loading,
                    #        and not just store the unparsed format strings to
                    #        be parsed during saving.
 		    ${CMAKE_SOURCE_DIR}/filters/libmso  # for NumberFormatParser
)

########### libkoodf2 ###############

set(koodf2_LIB_SRCS
    Odf2Debug.cpp

    # Storage objects, using old DOM based loading
    KoXmlStreamReader.cpp
    KoXmlUtils.cpp

    KoTable.cpp
    KoRow.cpp
    KoColumn.cpp
    KoCell.cpp
    KoCellValue.cpp
    KoDummyCellValue.cpp
    KoCellChild.cpp
    KoRawCellChild.cpp

    # Style objects, using old DOM based loading
    KoStyle.cpp
    KoTblStyle.cpp
    KoRowStyle.cpp
    KoColumnStyle.cpp
    KoCellStyle.cpp

    # Style objects, using newer stream based loading
    KoOdfStyleBase.cpp
    KoOdfStyleManager.cpp
    KoOdfStyle.cpp
    KoOdfListStyle.cpp
    KoOdfPageLayout.cpp
    KoOdfStyleProperties.cpp
    KoOdfTextProperties.cpp
    KoOdfParagraphProperties.cpp
    KoOdfListLevelProperties.cpp
    KoOdfGraphicProperties.cpp
    KoOdfPageLayoutProperties.cpp
    KoOdfHeaderFooterProperties.cpp

    chart/KoOdfChartWriter.cpp
)

add_library(koodf2 SHARED ${koodf2_LIB_SRCS})
generate_export_header(koodf2 BASE_NAME koodf2)

target_link_libraries(koodf2
    PUBLIC
        koodf
    PRIVATE
        mso
)

set_target_properties(koodf2 PROPERTIES
    VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)
install(TARGETS koodf2 ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


if (SHOULD_BUILD_DEVEL_HEADERS)

install( FILES
    ${CMAKE_CURRENT_BINARY_DIR}/koodf2_export.h

    KoStyle.h
    KoTable.h
    KoTblStyle.h
    KoColumn.h
    KoColumnStyle.h
    KoRawCellChild.h
    KoRow.h
    KoRowStyle.h
    KoCell.h
    KoCellStyle.h

    KoOdfStyleBase.h
    KoOdfStyleManager.h
    KoOdfStyle.h
    KoOdfStyleProperties.h
    KoOdfTextProperties.h
    KoOdfParagraphProperties.h
    KoOdfGraphicProperties.h
    KoOdfPageLayoutProperties.h
    KoOdfHeaderFooterProperties.h

    chart/Charting.h
    chart/KoOdfChartWriter.h

DESTINATION ${KDE_INSTALL_INCLUDEDIR}/calligra COMPONENT Devel)

endif()
