#add_subdirectory( tests )

include_directories( ${KOODF_INCLUDES} ${KOODF2_INCLUDES} )

########### libkoodf2 ###############

set(koodf2_LIB_SRCS
    KoXmlStreamReader.cpp
    KoTable.cpp
    KoRow.cpp
    KoColumn.cpp
    KoCell.cpp
    KoCellValue.cpp
    KoDummyCellValue.cpp
    KoCellChild.cpp
    KoRawCellChild.cpp

    KoStyle.cpp
    KoTblStyle.cpp
    KoRowStyle.cpp
    KoColumnStyle.cpp
    KoCellStyle.cpp

    KoOdfStyle.cpp
    KoOdfStyleProperties.cpp
    KoOdfTextProperties.cpp
    KoOdfParagraphProperties.cpp
    KoOdfGraphicProperties.cpp
    KoOdfStyleManager.cpp
)

kde4_add_library(koodf2 SHARED ${koodf2_LIB_SRCS})

target_link_libraries(koodf2 koodf ${KDE4_KIO_LIBS} ${QT_QTXML_LIBRARY})
target_link_libraries(koodf2 LINK_INTERFACE_LIBRARIES ${KDE4_KIO_LIBS})

set_target_properties(koodf2 PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS koodf2 ${INSTALL_TARGETS_DEFAULT_ARGS} )


install( FILES
    koodf2_export.h
    KoCell.h
    KoCellStyle.h
    KoColumn.h
    KoColumnStyle.h
    KoOdfGraphicProperties.h
    KoOdfParagraphProperties.h
    KoOdfStyle.h
    KoOdfStyleManager.h
    KoOdfStyleProperties.h
    KoOdfTextProperties.h
    KoRawCellChild.h
    KoRow.h
    KoRowStyle.h
    KoStyle.h
    KoTable.h
    KoTblStyle.h
DESTINATION ${INCLUDE_INSTALL_DIR}/calligra COMPONENT Devel)

