set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${KOTEXT_INCLUDES} )

if(MSVC OR (WIN32 AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel"))
  # avoid "cannot open file 'LIBC.lib'" error
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBC.LIB")
endif()

# call: kotextlayout_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(KOTEXTLAYOUT_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "libs-kotextlayout-"
    )
endmacro()

########### next target ###############

set(TestDocumentLayout_test_SRCS
 TestDocumentLayout.cpp
 MockRootAreaProvider.cpp
)
kotextlayout_add_unit_test(TestDocumentLayout ${TestDocumentLayout_test_SRCS}  LINK_LIBRARIES kotext kotextlayout Qt6::Test)

########### next target ###############

set(TestBlockLayout_test_SRCS
 TestBlockLayout.cpp
 MockRootAreaProvider.cpp
)
kotextlayout_add_unit_test(TestBlockLayout ${TestBlockLayout_test_SRCS}  LINK_LIBRARIES kotext kotextlayout Qt6::Test)

########### next target ###############

set(TestTableLayout_test_SRCS
 TestTableLayout.cpp
 MockRootAreaProvider.cpp
)
kotextlayout_add_unit_test(TestTableLayout ${TestTableLayout_test_SRCS}  LINK_LIBRARIES kotext kotextlayout Qt6::Test)
