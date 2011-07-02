include_directories( ${KOSVG_INCLUDES} ${KOODF_INCLUDES} ${FLAKE_INCLUDES} )

set(kosvg_LIB_SRCS
    SvgUtil.cpp
    SvgGraphicContext.cpp
    SvgSavingContext.cpp
    SvgWriter.cpp
    SvgStyleWriter.cpp
    SvgSerializable.cpp
)

kde4_add_library(kosvg SHARED ${kosvg_LIB_SRCS})

target_link_libraries(kosvg koodf flake ${QT_QTXML_LIBRARY})

set_target_properties(kosvg PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS kosvg ${INSTALL_TARGETS_DEFAULT_ARGS} )
