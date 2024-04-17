set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${KOPAGEAPP_INCLUDES} )

# call: kopageapp_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(KOPAGEAPP_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "libs-flake-"
    )
endmacro()

########### next target ###############

kopageapp_add_unit_test(TestPAPageInsertCommand TestPAPageInsertCommand.cpp  LINK_LIBRARIES kopageapp Qt6::Test)

########### next target ###############

kopageapp_add_unit_test(TestPAPageDeleteCommand TestPAPageDeleteCommand.cpp  LINK_LIBRARIES kopageapp Qt6::Test)

########### next target ###############

kopageapp_add_unit_test(TestPAPageMoveCommand TestPAPageMoveCommand.cpp  LINK_LIBRARIES kopageapp Qt6::Test)

########### next target ###############

kopageapp_add_unit_test(TestPACopyPastePage TestPACopyPastePage.cpp  LINK_LIBRARIES kopageapp Qt6::Test)
