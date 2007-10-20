set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
add_definitions(-DCOMPILING_TESTS)

include_directories( ${KOGUIUTILS_INCLUDES} ${KOTEXT_INCLUDES} ${KDE4_INCLUDES} ${CMAKE_SOURCE_DIR}/libs/flake/tests
        ${CMAKE_SOURCE_DIR}/kword/part)

########### next target ###############

set(TestPageManager_test_SRCS TestPageManager.cpp)
kde4_add_unit_test(TestPageManager TESTNAME kword-part-TestPageManager  ${TestPageManager_test_SRCS})
target_link_libraries(TestPageManager kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestClipToPage_test_SRCS TestClipToPage.cpp)
kde4_add_unit_test(TestClipToPage TESTNAME kword-part-TestClipToPage  ${TestClipToPage_test_SRCS})
target_link_libraries(TestClipToPage kwordprivate ${QT_QTTEST_LIBRARY})

########### end ###############
