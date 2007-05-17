if( KDE4_BUILD_TESTS )

########### AttributeManager Test ###############

set( TestAttributeManager_SRCS TestAttributeManager.cpp )
kde4_automoc( ${TestAttributeManager_SRCS} )
kde4_add_executable( TestAttributeManager RUN_UNINSTALLED ${TestAttributeManager_SRCS} )
target_link_libraries( TestAttributeManager kformulalib ${QT_QTTEST_LIBRARY} )
add_test(kformula-AttributeManager TestAttributeManager )

########### Load and Save Test ###############

set(TestLoadAndSave_SRCS TestLoadAndSave.cpp)
kde4_automoc(${TestLoadAndSave_SRCS})
kde4_add_executable(TestLoadAndSave RUN_UNINSTALLED ${TestLoadAndSave_SRCS})
target_link_libraries(TestLoadAndSave kformulalib ${QT_QTTEST_LIBRARY})
add_test(kformula-LoadAndSave TestLoadAndSave)

endif( KDE4_BUILD_TESTS )
