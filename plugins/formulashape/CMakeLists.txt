add_definitions(-DTRANSLATION_DOMAIN=\"calligra_shape_formula\")

# To hide all the warnings from the generated itex2MML code
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
    add_definitions(-Wno-unused-label)
    add_definitions(-Wno-sign-compare)
    add_definitions(-Wno-unused-function)
endif ()

set(ITEXTOMML_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/itexToMML")

include_directories( ${CMAKE_SOURCE_DIR}/plugins/formulashape
                     ${CMAKE_SOURCE_DIR}/plugins/formulashape/elements
                     
                     ${KOMAIN_INCLUDES}
)

########### KoFormula Library ###############

set( koformula_LIB_SRCS
   FormulaDebug.cpp
   AttributeManager.cpp
   FormulaRenderer.cpp
   FormulaData.cpp
   FormulaCursor.cpp
   FormulaEditor.cpp
   FormulaCommand.cpp
   ElementFactory.cpp 
   Dictionary.cpp

   elements/BasicElement.cpp
   elements/FixedElement.cpp
   elements/RowElement.cpp
   elements/FormulaElement.cpp 
   elements/TextElement.cpp
   elements/FractionElement.cpp
   elements/SpaceElement.cpp
   elements/GlyphElement.cpp
   elements/IdentifierElement.cpp
   elements/OperatorElement.cpp
   elements/MultiscriptElement.cpp
   elements/SubSupElement.cpp
   elements/UnderOverElement.cpp
   elements/FencedElement.cpp 
   elements/TableElement.cpp
   elements/TableRowElement.cpp
   elements/TableDataElement.cpp
   elements/RootElement.cpp
   elements/EncloseElement.cpp
   elements/ActionElement.cpp
   elements/PaddedElement.cpp
   elements/ErrorElement.cpp
   elements/StyleElement.cpp
   elements/TokenElement.cpp
   elements/NumberElement.cpp
   elements/StringElement.cpp
   elements/PhantomElement.cpp
   elements/SquareRootElement.cpp
   elements/AnnotationElement.cpp
   elements/UnknownElement.cpp
)

add_library(koformula SHARED ${koformula_LIB_SRCS})
generate_export_header(koformula EXPORT_MACRO_NAME KOFORMULA_EXPORT)

target_link_libraries(koformula  PUBLIC komain)

set_target_properties(koformula PROPERTIES VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION} )

install(TARGETS koformula ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

########### FormulaShape Plugin ###############

find_package(Cauchy)

macro_optional_find_package(Cauchy)
set_package_properties(Cauchy PROPERTIES
    DESCRIPTION "Cauchy's M2MML, a Matlab/Octave to MathML compiler"
    URL "https://bitbucket.org/cyrille/cauchy"
    PURPOSE "Required for the matlab/octave formula tool"
    TYPE OPTIONAL
)

if(M2MML_FOUND)
    set(M2MML_TOOL_LIBRARIES ${M2MML_LIBRARY})
    include_directories(${CAUCHY_INCLUDE_DIR})
    add_definitions(-DHAVE_M2MML)
endif()

set( formulashape_PART_SRCS
   KoFormulaShapePlugin.cpp
   KoFormulaShape.cpp
   FormulaCommandUpdate.cpp
   KoFormulaShapeFactory.cpp
   KoFormulaTool.cpp
   FormulaCommand.cpp
   KoFormulaToolFactory.cpp
   FormulaToolWidget.cpp
   FormulaToolEditWidget.cpp
   FormulaDocument.cpp
   FormulaPart.cpp
)

if (NOT MSVC AND NOT (WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))
set (formulashape_PART_SRCS
   ${formulashape_PART_SRCS}
   KoM2MMLForumulaTool.cpp
   # itexToMML files
   ${ITEXTOMML_SOURCE_DIR}/lex.yy.c
   ${ITEXTOMML_SOURCE_DIR}/y.tab.c
)
endif ()

ki18n_wrap_ui( formulashape_PART_SRCS
    FormulaToolWidget.ui
    FormulaToolEditWidget.ui
)

add_library(calligra_shape_formula MODULE ${formulashape_PART_SRCS})

target_link_libraries(calligra_shape_formula koformula ${M2MML_TOOL_LIBRARIES})


#set_target_properties(koformula PROPERTIES VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION} )
install(TARGETS calligra_shape_formula  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/shapes)

add_subdirectory( pics ) 
# TODO: those font files seem unused currently, investigate if still needed
# add_subdirectory( fonts )
if(BUILD_TESTING)
    add_subdirectory( tests )
endif()
add_subdirectory( templates )
