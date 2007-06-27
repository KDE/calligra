include_directories( ${CMAKE_SOURCE_DIR}/shapes/text ${KOGUIUTILS_INCLUDES} ${KOTEXT_INCLUDES} )

########### next target ###############

set(TestDocumentLayout_test_SRCS
    TestDocumentLayout.cpp
    TestLists.cpp
    ../Layout.cpp
    ../ListItemsHelper.cpp
)
kde4_automoc(${TestDocumentLayout_test_SRCS})
kde4_add_unit_test(TestDocumentLayout TESTNAME shapes-text-TestDocumentLayout ${TestDocumentLayout_test_SRCS})
target_link_libraries(TestDocumentLayout kotext ${QT_QTTEST_LIBRARY})

########### next target ###############

set(TestChangesDatabase_test_SRCS
    TestChangesDatabase.cpp
    ../TextChanges.cpp
    ../TextChange.cpp
)
kde4_automoc(${TestChangesDatabase_test_SRCS})
kde4_add_unit_test(TestChangesDatabase TESTNAME shapes-text-TestChangesDatabase ${TestChangesDatabase_test_SRCS})
target_link_libraries(TestChangesDatabase ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY})

########### next target ###############

set(TestChangeListCommand_test_SRCS
    TestChangeListCommand.cpp
    ../commands/ChangeListCommand.cpp
    ../commands/TextCommandBase.cpp
)
kde4_automoc(${TestChangeListCommand_test_SRCS})
kde4_add_unit_test(TestChangeListCommand TESTNAME shapes-text-TestChangeListCommand ${TestChangeListCommand_test_SRCS})
target_link_libraries(TestChangeListCommand kotext ${QT_QTTEST_LIBRARY})

