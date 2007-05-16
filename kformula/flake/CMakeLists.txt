add_subdirectory( pics ) 
add_subdirectory( fonts ) 
#add_subdirectory( tests )

include_directories(${QT_INCLUDES} ${KOGUIUTILS_INCLUDES} )

########### KFormula Library ###############

set( kformulalib_PART_SRCS
   AttributeManager.cpp
   FormulaRenderer.cpp
   FormulaCursor.cpp 
   BasicElement.cpp 
   RowElement.cpp
   FormulaElement.cpp 
   TextElement.cpp
   FractionElement.cpp
   SpaceElement.cpp
   GlyphElement.cpp
   IdentifierElement.cpp
   OperatorElement.cpp
   OperatorDictionary.cpp
   Entities.cpp
   ElementFactory.cpp 
#   MultiscriptElement.cpp
   UnderOverElement.cpp
#   BracketElement.cpp 
#   MatrixElement.cpp
#   MatrixRowElement.cpp
#   MatrixEntryElement.cpp
   RootElement.cpp
#   EncloseElement.cpp
#   ActionElement.cpp
#   PaddedElement.cpp
#   ErrorElement.cpp
   TokenStyleElement.cpp
   StyleElement.cpp
   TokenElement.cpp
   NumberElement.cpp
#   MatrixDialog.cc 
   contextstyle.cc 
   symboltable.cc 
#   kformulaconfigpage.cc 
#   symbolaction.cc 
   fontstyle.cc 
)

kde4_automoc(${kformulalib_PART_SRCS})

kde4_add_library(kformulalib SHARED ${kformulalib_PART_SRCS})

target_link_libraries(kformulalib  ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} koguiutils)

install(TARGETS kformulalib DESTINATION ${LIB_INSTALL_DIR})

########### FormulaShape Plugin ###############

set( formulashape_PART_SRCS
   KoFormulaShapePlugin.cpp
   KoFormulaShape.cpp
   KoFormulaShapeFactory.cpp
   KoFormulaTool.cpp
   KoFormulaToolFactory.cpp
)

kde4_automoc(${formulashape_PART_SRCS})

kde4_add_plugin(formulashape ${formulashape_PART_SRCS})

target_link_libraries(formulashape  ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} koguiutils kformulaprivate)


#set_target_properties(kformulalib PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS formulashape  DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES formulashape.desktop DESTINATION ${SERVICES_INSTALL_DIR})

