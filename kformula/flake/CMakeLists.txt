remove_definitions(-DQT3_SUPPORT)
include_directories(${QT_INCLUDES} ${KOMAIN_INCLUDES}  )

########### KFormula Library ###############

set( kformulalib_PART_SRCS
   AttributeManager.cpp
   FormulaRenderer.cpp
   FormulaData.cpp
   FormulaCursor.cpp
   FormulaEditor.cpp
   FormulaCommand.cpp
   ElementFactory.cpp 
   BasicElement.cpp
   FixedElement.cpp
   EmptyElement.cpp
   RowElement.cpp
   FormulaElement.cpp 
   TextElement.cpp
   FractionElement.cpp
   SpaceElement.cpp
   GlyphElement.cpp
   IdentifierElement.cpp
   OperatorElement.cpp
   MultiscriptElement.cpp
   SubSupElement.cpp
   UnderOverElement.cpp
   FencedElement.cpp 
   TableElement.cpp
   TableRowElement.cpp
   TableEntryElement.cpp
   RootElement.cpp
   EncloseElement.cpp
   ActionElement.cpp
   PaddedElement.cpp
   ErrorElement.cpp
   StyleElement.cpp
   TokenElement.cpp
   NumberElement.cpp
   StringElement.cpp
   PhantomElement.cpp
   SquareRootElement.cpp
   UnknownElement.cpp
   Dictionary.cpp
)

kde4_add_library(kformulalib SHARED ${kformulalib_PART_SRCS})

target_link_libraries(kformulalib komain ${KDE4_KDEUI_LIBS} )
target_link_libraries(kformulalib LINK_INTERFACE_LIBRARIES komain)

set_target_properties(kformulalib PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS kformulalib ${INSTALL_TARGETS_DEFAULT_ARGS})

########### FormulaShape Plugin ###############

set( formulashape_PART_SRCS
   KoFormulaShapePlugin.cpp
   KoFormulaShape.cpp
   FormulaCommandUpdate.cpp
   KoFormulaShapeFactory.cpp
   KoFormulaTool.cpp
   FormulaCommand.cpp
   KoFormulaToolFactory.cpp
   FormulaToolWidget.cpp
)

kde4_add_ui_files( formulashape_PART_SRCS
    FormulaToolWidget.ui
)

kde4_add_plugin(formulashape ${formulashape_PART_SRCS})

target_link_libraries(
    formulashape
    kformulaprivate
    kformulalib
    komain
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    ${QT_QT3SUPPORT_LIBRARY}
    )


#set_target_properties(kformulalib PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS formulashape  DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES formulashape.desktop DESTINATION ${SERVICES_INSTALL_DIR})

add_subdirectory( pics ) 
add_subdirectory( fonts ) 
add_subdirectory( tests )
add_subdirectory( templates )
