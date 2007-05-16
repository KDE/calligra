if( KDE4_BUILD_TESTS )

#include_directories( ${CMAKE_SOURCE_DIR}/libs/kformula ${FLAKE_INCLUDES} ${QT_INCLUDES} ${KOSTORE_INCLUDES} )

#set( AttributeManager_test_SRCS
#     ../AttributeManager.cpp
#     ../BasicElement.cpp
#     ../FormulaCursor.cpp
# TestAttributeManager.cpp )

#kde4_automoc( ${AttributeManager_test_SRCS} )
#kde4_add_executable( TestAttributeManager RUN_UNINSTALLED ${AttributeManager_test_SRCS} )
#target_link_libraries( TestAttributeManager ${QT_QTTEST_LIBRARY} flake koguiutils )
#add_test( TestAttributeManager ${EXECUTABLE_OUTPUT_PATH}/TestAttributeManager )

########### BasicElement Test ###############

set(TestLoadAndSave_SRCS TestLoadAndSave.cpp)

kde4_automoc(${TestLoadAndSave_SRCS})

kde4_add_executable(TestLoadAndSave RUN_UNINSTALLED ${TestLoadAndSave_SRCS})

target_link_libraries(TestLoadAndSave kformulalib ${QT_QTTEST_LIBRARY})

add_test(kformula-LoadAndSave TestLoadAndSave)

endif( KDE4_BUILD_TESTS )
