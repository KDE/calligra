set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${KOODF_INCLUDES} )

# call: kostore_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(KOSTORE_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "libs-kostore-"
    )
endmacro()

########### next target ###############

set(lzftest_SRCS ../KoLZF.cpp TestKoLZF.cpp )
kostore_add_unit_test(TestKoLZF ${lzftest_SRCS}  LINK_LIBRARIES Qt6::Test)

########### next target ###############

set(xmlvectortest_SRCS ../KoLZF.cpp TestKoXmlVector.cpp )
kostore_add_unit_test(TestKoXmlVector ${xmlvectortest_SRCS}  LINK_LIBRARIES Qt6::Test)

########### next target ###############

set(storedroptest_SRCS storedroptest.cpp )
add_executable(storedroptest ${storedroptest_SRCS})
ecm_mark_as_test(storedroptest)
target_link_libraries(storedroptest kostore KF6::I18n Qt6::Widgets)


