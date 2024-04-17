set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories(
    ${KOTEXT_INCLUDES}
)

add_definitions(-DSPECS_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/specs/")

# call: kotext_styles_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(KOTEXT_STYLES_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "libs-kotext-styles-"
    )
endmacro()

########### next target ###############

kotext_styles_add_unit_test(TestStyles TestStyles.cpp  LINK_LIBRARIES kotext Qt6::Test)

########### next target ###############

kotext_styles_add_unit_test(TestListStyle TestListStyle.cpp  LINK_LIBRARIES kotext Qt6::Test)

########### next target ###############

kotext_styles_add_unit_test(TestSectionStyle TestSectionStyle.cpp  LINK_LIBRARIES kotext Qt6::Test)

########### next target ###############

kotext_styles_add_unit_test(TestTableCellStyle TestTableCellStyle.cpp  LINK_LIBRARIES kotext Qt6::Test)

########### next target ###############

kotext_styles_add_unit_test(TestStyleManager TestStyleManager.cpp  LINK_LIBRARIES kotext Qt6::Test)

########### next target ###############

#kotext_styles_add_unit_test(TestOpenDocumentStyle TestOpenDocumentStyle.cpp  LINK_LIBRARIES kotext Qt6::Test)
