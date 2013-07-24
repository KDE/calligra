set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/libs/kostep
    )
include_directories( ${KOSTEP_INCLUDES} ${QT_INCLUDES})

########### next target ###############

set(TestCreateAndStoreSteps_test_SRCS TestCreateAndStoreSteps.cpp )

kde4_add_unit_test(TestCreateAndStoreSteps TESTNAME libs-kostep-TestCreateAndStoreSteps ${TestCreateAndStoreSteps_test_SRCS})

target_link_libraries(TestCreateAndStoreSteps kostep ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
#target_link_libraries(TestCreateAndStoreSteps ${QT_QTTEST_LIBRARY})
#target_link_libraries(TestCreateAndStoreSteps ${QT_QTCORE_LIBRARY})
#target_link_libraries(TestCreateAndStoreSteps ${QT_QTGUI_LIBRARY})
