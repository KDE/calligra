enable_testing()

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

# call: formulashape_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(FORMULASHAPE_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "shapes-formula-"
    )
endmacro()

########### AttributeManager Test ###############

formulashape_add_unit_test(AttributeManager
    TestAttributeManager.cpp
    LINK_LIBRARIES koformula Qt6::Test
)

########### Load Test ###############

formulashape_add_unit_test(Load
    TestLoad.cpp
    LINK_LIBRARIES koformula Qt6::Test
)

########### Load and Save Test ###############

formulashape_add_unit_test(LoadAndSave
    TestLoadAndSave.cpp
    LINK_LIBRARIES koformula Qt6::Test
)

########### Layout Test ###############

# formulashape_add_unit_test(Layout
#     TestLayout.cpp
#     LINK_LIBRARIES koformula Qt6::Test
# )

########### Cursor Test ###############

set(TestCursor_SRCS
TestCursor.cpp
   ../FormulaPart.cpp
   ../KoFormulaShape.cpp
   ../FormulaDocument.cpp
   ../FormulaToolWidget.cpp
   ../FormulaToolEditWidget.cpp
   ../FormulaCommandUpdate.cpp
   ../KoFormulaShapeFactory.cpp
   ../KoFormulaTool.cpp
   ../FormulaCommand.cpp
)
if(NOT WIN32)
set(TestCursor_SRCS
    ${TestCursor_SRCS}
    ../FormulaCursor.cpp)
endif()

qt_wrap_ui( TestCursor_SRCS
    ../FormulaToolWidget.ui
)

formulashape_add_unit_test(Cursor
    ${TestCursor_SRCS}
    LINK_LIBRARIES koformula KF6::KIOWidgets Qt6::Test
)
