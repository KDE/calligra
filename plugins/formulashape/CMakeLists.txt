add_definitions(-DTRANSLATION_DOMAIN=\"calligra_shape_formular\")

# To hide all the warnings from the generated itex2MML code
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
    add_definitions(-Wno-unused-label)
    add_definitions(-Wno-sign-compare)
    add_definitions(-Wno-unused-function)
endif ()

set(ITEXTOMML_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/itexToMML")

include_directories( ${CMAKE_SOURCE_DIR}/plugins/formulashape
                     ${LIBMATHVIEW_INCLUDE_DIRS}
                     ${KOMAIN_INCLUDES}
                     ${LIBXSLT_INCLUDE_DIR}
                     ${LIBXML2_INCLUDE_DIR}
)

#generate_export_header(koformula EXPORT_MACRO_NAME KOFORMULA_EXPORT)


########### FormulaShape Plugin ###############

find_package(Cauchy)

macro_optional_find_package(Cauchy)
macro_log_feature(M2MML_FOUND "Cauchy's M2MML" "Matlab/Octave to MathML compiler" "https://bitbucket.org/cyrille/cauchy" FALSE "" "Required for the matlab/octave formula tool")

if(M2MML_FOUND)
    set(M2MML_TOOL_LIBRARIES ${M2MML_LIBRARY})
    include_directories(${CAUCHY_INCLUDE_DIR})
    add_definitions(-DHAVE_M2MML)
endif()

set(formulashape_PART_SRCS
    FormulaDebug.cpp
    KoFormulaShapePlugin.cpp
    KoFormulaShape.cpp
    KoFormulaShapeFactory.cpp
    KoFormulaTool.cpp
    FormulaFontFamilyAction.cpp
    ../textshape/FontSizeAction.cpp
    FormulaCommand.cpp
    KoFormulaToolFactory.cpp
    FormulaDocument.cpp
    FormulaPart.cpp
    BasicXMLSyntaxHighlighter.cpp
    # itexToMML files
    ${ITEXTOMML_SOURCE_DIR}/lex.yy.c
    ${ITEXTOMML_SOURCE_DIR}/y.tab.c
)

add_library(calligra_shape_formular MODULE ${formulashape_PART_SRCS})
kcoreaddons_desktop_to_json(calligra_shape_formular calligra_shape_formular.desktop)
target_compile_options(calligra_shape_formular PRIVATE -Wno-error=implicit-function-declaration)
target_link_libraries(calligra_shape_formular komain ${LIBMATHVIEW_LIBRARIES} ${M2MML_TOOL_LIBRARIES} ${LIBXSLT_LIBRARIES} ${LIBXML2_LIBRARIES})


#set_target_properties(koformula PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )
install(TARGETS calligra_shape_formular  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/xsltml/mmltex.xsl
        DESTINATION ${DATA_INSTALL_DIR}/calligra/formula)

add_subdirectory( pics ) 
add_subdirectory( fonts )
add_subdirectory( templates )
