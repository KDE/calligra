set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories(${CMAKE_SOURCE_DIR}/kexiutils)

#add_definitions(-DFILES_DATA_DIR="\\"${CMAKE_CURRENT_SOURCE_DIR}/data/\\"")

########### next target ###############

kde4_add_unit_test(tst_identifier TESTNAME KexiUtilsTest tst_identifier.cpp)
target_link_libraries(tst_identifier ${QT_QTTEST_LIBRARY} ${KDE4_KDECORE_LIBS} kexiutils)
