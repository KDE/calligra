set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

if(KDE4_BUILD_TESTS)

add_definitions(-DCOMPILING_TESTS)

include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} ${KOTEXT_INCLUDES} ${KDE4_INCLUDES}
    ${QT_INCLUDES} ${CMAKE_SOURCE_DIR}/kword/part ${CMAKE_SOURCE_DIR}/kword/part/frames
    ${CMAKE_SOURCE_DIR}/libs/flake/tests
)

########### next target ###############

set(KWordTestBasicLayout_test_SRCS TestBasicLayout.cpp)
kde4_add_unit_test(KWordTestBasicLayout TESTNAME kword-part-frame-TestBasicLayout  ${KWordTestBasicLayout_test_SRCS})
target_link_libraries(KWordTestBasicLayout kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(KWordTestDocumentLayout_test_SRCS TestDocumentLayout.cpp)
kde4_add_unit_test(KWordTestDocumentLayout TESTNAME kword-part-frame-TestDocumentLayout  ${KWordTestDocumentLayout_test_SRCS})
target_link_libraries(KWordTestDocumentLayout kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(KWordTestTextFrameSorting_test_SRCS TestTextFrameSorting.cpp)
kde4_add_unit_test(KWordTestTextFrameSorting TESTNAME kword-part-frame-TestTextFrameSorting  ${KWordTestTextFrameSorting_test_SRCS})
target_link_libraries(KWordTestTextFrameSorting kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

endif(KDE4_BUILD_TESTS)
