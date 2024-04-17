set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${KORDF_INCLUDES} )

# call: kordf_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(KORDF_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "libs-kordf-"
    )
endmacro()

kordf_add_unit_test(TestKoDocumentRdf TestKoDocumentRdf.cpp  LINK_LIBRARIES kordf Qt6::Test)

kordf_add_unit_test(rdf_test rdf_test.cpp  LINK_LIBRARIES kordf Qt6::Test)
