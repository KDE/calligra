set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
add_definitions(-DCOMPILING_TESTS)

include_directories(${KOTEXT_INCLUDES} 
        ${CMAKE_SOURCE_DIR}/libs/flake/tests
        ${CMAKE_BINARY_DIR}/libs/main
        ${CMAKE_SOURCE_DIR}/libs/kotext/tests
        ${CMAKE_SOURCE_DIR}/words/part
)

add_definitions(-DFILES_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data/")
add_definitions(-DFILES_OUTPUT_DIR="${CMAKE_CURRENT_BINARY_DIR}/")

########### next target ###############

#set(TestPageManager_test_SRCS TestPageManager.cpp)
#add_executable(TestPageManagerNAME words-part-TestPageManager  ${TestPageManager_test_SRCS})
add_test(TestPageManager TestPageManager)
ecm_mark_as_test(TestPageManager)
#target_link_libraries(TestPageManager wordsprivate Qt5::Test)

########### next target ###############

set(TestClipToPage_test_SRCS TestClipToPage.cpp)
add_executable(TestClipToPageNAME words-part-TestClipToPage  ${TestClipToPage_test_SRCS})
add_test(TestClipToPage TestClipToPage)
ecm_mark_as_test(TestClipToPage)
target_link_libraries(TestClipToPage wordsprivate Qt5::Test)

########### next target ###############

#set(TestViewMode_test_SRCS TestViewMode.cpp)
#add_executable(TestViewModeNAME words-part-TestViewMode  ${TestViewMode_test_SRCS})
add_test(TestViewMode TestViewMode)
ecm_mark_as_test(TestViewMode)
#target_link_libraries(TestViewMode wordsprivate Qt5::Test)

########### next target ###############

#set(TestPageTextInfo_test_SRCS TestPageTextInfo.cpp)
#add_executable(TestPageTextInfoNAME words-part-TestPageTextInfo  ${TestPageTextInfo_test_SRCS})
add_test(TestPageTextInfo TestPageTextInfo)
ecm_mark_as_test(TestPageTextInfo)
#target_link_libraries(TestPageTextInfo wordsprivate Qt5::Test)

########### next target ###############

#set(TestFrameLayout_test_SRCS TestFrameLayout.cpp)
#add_executable(TestFrameLayoutNAME words-part-frame-TestFrameLayout  ${TestFrameLayout_test_SRCS})
add_test(TestFrameLayout TestFrameLayout)
ecm_mark_as_test(TestFrameLayout)
#target_link_libraries(TestFrameLayout wordsprivate Qt5::Test)

########### next target ###############

#set(TestTextFrameSorting_test_SRCS TestTextFrameSorting.cpp)
#add_executable(TestTextFrameSortingNAME words-part-frame-TestTextFrameSorting  ${TestTextFrameSorting_test_SRCS})
add_test(TestTextFrameSorting TestTextFrameSorting)
ecm_mark_as_test(TestTextFrameSorting)
#target_link_libraries(TestTextFrameSorting wordsprivate Qt5::Test)

########### next target ###############
#set(TestTextFrameManagement_test_SRCS TestTextFrameManagement.cpp)
#add_executable(TestTextFrameManagementNAME words-part-frame-TestTextFrameManagement  ${TestTextFrameManagement_test_SRCS})
add_test(TestTextFrameManagement TestTextFrameManagement)
ecm_mark_as_test(TestTextFrameManagement)
#target_link_libraries(TestTextFrameManagement wordsprivate Qt5::Test)

########### next target ###############

if( SHOULD_BUILD_FEATURE_RDF )
#  set(TestRdf_SRCS TestRdf.cpp )
#  add_executable(TestRdfNAME words-part-TestRdf ${TestRdf_SRCS})
add_test(TestRdf TestRdf)
ecm_mark_as_test(TestRdf)
#  target_link_libraries(TestRdf wordsprivate Qt5::Test)
endif()


