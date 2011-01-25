include(MacroAddCompileFlags)
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_SOURCE_DIR}/kexi/kexiutils ${KDE4_INCLUDES} )

########### StaticSetOfStrings ###############

set(TestStaticSetOfStrings_SRCS TestStaticSetOfStrings.cpp)
kde4_add_unit_test(TestStaticSetOfStrings TESTNAME kexi-TestStaticSetOfStrings ${TestStaticSetOfStrings_SRCS})
target_link_libraries(TestStaticSetOfStrings kexiutils ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})