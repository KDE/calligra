set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/kexiutils)

#add_definitions(-DFILES_DATA_DIR=${CMAKE_CURRENT_SOURCE_DIR}/data/)

########### next target ###############

kde4_add_unit_test(TestIdentifier TESTNAME TestIdentifier TestIdentifier.cpp)
target_link_libraries(TestIdentifier ${QT_QTTEST_LIBRARY} ${KDE4_KDECORE_LIBS} kexiutils)
