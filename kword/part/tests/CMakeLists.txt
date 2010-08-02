set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
add_definitions(-DCOMPILING_TESTS)

include_directories(  ${KOTEXT_INCLUDES} ${KDE4_INCLUDES} ${CMAKE_SOURCE_DIR}/libs/flake/tests
        ${CMAKE_SOURCE_DIR}/kword/part)

add_definitions(-DFILES_DATA_DIR="\\"${CMAKE_CURRENT_SOURCE_DIR}/data/\\"")

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

set(TestFrameLayout_test_SRCS TestFrameLayout.cpp)
kde4_add_unit_test(TestFrameLayout TESTNAME kword-part-frame-TestFrameLayout  ${TestFrameLayout_test_SRCS})
target_link_libraries(TestFrameLayout kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestDocumentLayout_test_SRCS TestDocumentLayout.cpp)
kde4_add_unit_test(TestDocumentLayout TESTNAME kword-part-frame-TestDocumentLayout  ${TestDocumentLayout_test_SRCS})
target_link_libraries(TestDocumentLayout kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestTextFrameSorting_test_SRCS TestTextFrameSorting.cpp)
kde4_add_unit_test(TestTextFrameSorting TESTNAME kword-part-frame-TestTextFrameSorting  ${TestTextFrameSorting_test_SRCS})
target_link_libraries(TestTextFrameSorting kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############
set(TestTextFrameManagement_test_SRCS TestTextFrameManagement.cpp)
kde4_add_unit_test(TestTextFrameManagement TESTNAME kword-part-frame-TestTextFrameManagement  ${TestTextFrameManagement_test_SRCS})
target_link_libraries(TestTextFrameManagement kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

if( Soprano_FOUND )
  set(TestRdf_SRCS TestRdf.cpp )
  kde4_add_unit_test(TestRdf TESTNAME kword-part-TestRdf ${TestRdf_SRCS})
  target_link_libraries(TestRdf kwordprivate ${QT_QTTEST_LIBRARY})
endif( Soprano_FOUND )

########### end ###############
