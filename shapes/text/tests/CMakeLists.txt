if(KDE4_BUILD_TESTS)

include_directories( ${CMAKE_SOURCE_DIR}/shapes/text ${KOFFICEUI_INCLUDES} ${KOTEXT_INCLUDES} )

########### next target ###############

set(TestDocumentLayout_test_SRCS
    TestDocumentLayout.cpp
    TestLists.cpp
    ../Layout.cpp
    ../ListItemsHelper.cpp
)
kde4_automoc(${TestDocumentLayout_test_SRCS})
kde4_add_executable(TestDocumentLayout RUN_UNINSTALLED ${TestDocumentLayout_test_SRCS})
target_link_libraries(TestDocumentLayout kotext ${QT_QTTEST_LIBRARY})
add_test(TestDocumentLayout ${EXECUTABLE_OUTPUT_PATH}/TestDocumentLayout)

endif(KDE4_BUILD_TESTS)
