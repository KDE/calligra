ENABLE_TESTING()

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
########### AttributeManager Test ###############

 set( TestAttributeManager_SRCS TestAttributeManager.cpp )
 kde4_add_unit_test( TestAttributeManager TESTNAME kformula-AttributeManager  ${TestAttributeManager_SRCS} )
 target_link_libraries( TestAttributeManager kformulalib ${QT_QTTEST_LIBRARY} )

########### Load Test ###############

set(TestLoad_SRCS TestLoad.cpp)
kde4_add_unit_test(TestLoad TESTNAME kformula-Load  ${TestLoad_SRCS})
target_link_libraries(TestLoad kformulalib ${QT_QTTEST_LIBRARY})

########### Load and Save Test ###############

set(TestLoadAndSave_SRCS TestLoadAndSave.cpp)
kde4_add_unit_test(TestLoadAndSave TESTNAME kformula-LoadAndSave  ${TestLoadAndSave_SRCS})
target_link_libraries(TestLoadAndSave kformulalib ${QT_QTTEST_LIBRARY})

########### Layout Test ###############

set(TestLayout_SRCS TestLayout.cpp)
kde4_add_unit_test(TestLayout TESTNAME kformula-LoadAndSave  ${TestLayout_SRCS})
target_link_libraries(TestLayout kformulalib ${QT_QTTEST_LIBRARY})
