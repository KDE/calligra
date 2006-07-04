
if(KDE4_BUILD_TESTS)

add_definitions(-DCOMPILING_TESTS)

include_directories( ${KOFFICECORE_INCLUDES} ${FLAKE_INCLUDES} ${KOTEXT_INCLUDES} ${KDE4_INCLUDES}
    ${QT_INCLUDES} ${CMAKE_SOURCE_DIR}/kword/part ${CMAKE_SOURCE_DIR}/kword/part/frame)

########### next target ###############

set(TestBasicLayout_test_SRCS TestBasicLayout.cpp)
kde4_automoc(${TestBasicLayout_test_SRCS})
kde4_add_executable(TestBasicLayout RUN_UNINSTALLED ${TestBasicLayout_test_SRCS})
target_link_libraries(TestBasicLayout kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestDocumentLayout_test_SRCS TestDocumentLayout.cpp)
kde4_automoc(${TestDocumentLayout_test_SRCS})
kde4_add_executable(TestDocumentLayout RUN_UNINSTALLED ${TestDocumentLayout_test_SRCS})
target_link_libraries(TestDocumentLayout kwordprivate ${QT_QTTEST_LIBRARY})

########### next target ###############

endif(KDE4_BUILD_TESTS)
