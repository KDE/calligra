set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/kexiutils)

#add_definitions(-DFILES_DATA_DIR=${CMAKE_CURRENT_SOURCE_DIR}/data/)

########### next target ###############

add_executable(TestIdentifierNAME TestIdentifier TestIdentifier.cpp)
add_test(TestIdentifier TestIdentifier)
ecm_mark_as_test(TestIdentifier)
target_link_libraries(TestIdentifier Qt5::Test  kexiutils)
