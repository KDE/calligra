include_directories( ${FLAKE_INCLUDES} )

########### Flake Plugin library ###############

SET (formulashape2g_LIB_SRCS

    qtmmlwidget/qtmmlwidget.cpp

    FormulaShapePlugin.cpp
    FormulaShapeFactory.cpp
    FormulaShape.cpp

    FormulaToolFactory.cpp
    FormulaTool.cpp
    FormulaDocument.cpp
    FormulaPart.cpp
    FormulaCursor.cpp

    ChangeFormulaCommand.cpp
    AlterFormulaCommand.cpp
)

kde4_add_plugin(formulashape2g ${formulashape2g_LIB_SRCS})

target_link_libraries(formulashape2g flake komain ${KDE4_KFILE_LIBRARY})

install(TARGETS formulashape2g DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES formulashape2g.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS formulashape2g DESTINATION ${PLUGIN_INSTALL_DIR})

kde4_install_icons( ${DATA_INSTALL_DIR}/calligra/icons )
