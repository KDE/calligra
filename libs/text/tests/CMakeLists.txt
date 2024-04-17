set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${KOTEXT_INCLUDES} )

if(MSVC OR (WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))
  # avoid "cannot open file 'LIBC.lib'" error
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBC.LIB")
endif()

# call: kotext_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(KOTEXT_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "libs-kotext-"
    )
endmacro()

########### next target ###############

kotext_add_unit_test(TestBorder TestBorder.cpp  LINK_LIBRARIES kotext Qt6::Test)

########### next target ###############

kotext_add_unit_test(TestKoTableColumnAndRowStyleManager TestKoTableColumnAndRowStyleManager.cpp  LINK_LIBRARIES kotext Qt6::Test)

########### next target ###############

kotext_add_unit_test(TestKoTextEditor TestKoTextEditor.cpp  LINK_LIBRARIES kotext Qt6::Test)

########### next target ###############

kotext_add_unit_test(TestKoBookmarkManager TestKoBookmarkManager.cpp  LINK_LIBRARIES kotext Qt6::Test)

########### next target ###############

kotext_add_unit_test(TestKoInlineTextObjectManager TestKoInlineTextObjectManager.cpp  LINK_LIBRARIES kotext Qt6::Test)
