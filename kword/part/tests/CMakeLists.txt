set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
add_definitions(-DCOMPILING_TESTS)

include_directories(  ${KOTEXT_INCLUDES} ${KDE4_INCLUDES} ${CMAKE_SOURCE_DIR}/libs/flake/tests
        ${CMAKE_SOURCE_DIR}/kword/part)

########### next target ###############

set(TestPageManager_test_SRCS TestPageManager.cpp)
kde4_add_unit_test(TestPageManager TESTNAME kword-part-TestPageManager  ${TestPageManager_test_SRCS})
target_link_libraries(TestPageManager kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestClipToPage_test_SRCS TestClipToPage.cpp)
kde4_add_unit_test(TestClipToPage TESTNAME kword-part-TestClipToPage  ${TestClipToPage_test_SRCS})
target_link_libraries(TestClipToPage kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestPageCommands_test_SRCS TestPageCommands.cpp)
kde4_add_unit_test(TestPageCommands TESTNAME kword-part-TestPageCommands  ${TestPageCommands_test_SRCS})
target_link_libraries(TestPageCommands kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestViewMode_test_SRCS TestViewMode.cpp)
kde4_add_unit_test(TestViewMode TESTNAME kword-part-TestViewMode  ${TestViewMode_test_SRCS})
target_link_libraries(TestViewMode kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestPageTextInfo_test_SRCS TestPageTextInfo.cpp)
kde4_add_unit_test(TestPageTextInfo TESTNAME kword-part-TestPageTextInfo  ${TestPageTextInfo_test_SRCS})
target_link_libraries(TestPageTextInfo kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestKWDocument_test_SRCS TestKWDocument.cpp)
kde4_add_unit_test(TestKWDocument TESTNAME kword-part-TestKWDocument  ${TestKWDocument_test_SRCS})
target_link_libraries(TestKWDocument kwordprivate ${QT_QTTEST_LIBRARY})

########### end ###############
