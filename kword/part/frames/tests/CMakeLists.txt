
if(KDE4_BUILD_TESTS)

add_definitions(-DCOMPILING_TESTS)

include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} ${KOTEXT_INCLUDES} ${KDE4_INCLUDES}
    ${QT_INCLUDES} ${CMAKE_SOURCE_DIR}/kword/part ${CMAKE_SOURCE_DIR}/kword/part/frames
    ${CMAKE_SOURCE_DIR}/libs/flake/tests
)

########### next target ###############

set(TestBasicLayout_test_SRCS TestBasicLayout.cpp)
kde4_automoc(${TestBasicLayout_test_SRCS})
kde4_add_unit_test(TestBasicLayout TESTNAME kword-part-frame-TestBasicLayout  ${TestBasicLayout_test_SRCS})
target_link_libraries(TestBasicLayout kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestDocumentLayout_test_SRCS TestDocumentLayout.cpp)
kde4_automoc(${TestDocumentLayout_test_SRCS})
kde4_add_unit_test(TestDocumentLayout TESTNAME kword-part-frame-TestDocumentLayout  ${TestDocumentLayout_test_SRCS})
target_link_libraries(TestDocumentLayout kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestTextFrameSorting_test_SRCS TestTextFrameSorting.cpp)
kde4_automoc(${TestTextFrameSorting_test_SRCS})
kde4_add_unit_test(TestTextFrameSorting TESTNAME kword-part-frame-TestTextFrameSorting  ${TestTextFrameSorting_test_SRCS})
target_link_libraries(TestTextFrameSorting kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

endif(KDE4_BUILD_TESTS)
