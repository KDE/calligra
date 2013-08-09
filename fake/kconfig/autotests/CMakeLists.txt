
remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_KEYWORDS)

include(ECMMarkAsTest)

find_package(Qt5 5.2.0 REQUIRED NO_MODULE COMPONENTS Concurrent Test Widgets)

macro(KCONFIGCORE_UNIT_TESTS)
   foreach(_testname ${ARGN})
      add_executable(${_testname} ${_testname}.cpp) # TODO NOGUI
      add_test(kconfigcore-${_testname} ${_testname})
      target_link_libraries(${_testname} KConfigCore Qt5::Test Qt5::Concurrent)
      ecm_mark_as_test(${_testname})
   endforeach()
endmacro()

macro(KCONFIGGUI_UNIT_TESTS)
   foreach(_testname ${ARGN})
      add_executable(${_testname} ${_testname}.cpp)
      add_test(kconfiggui-${_testname} ${_testname})
      target_link_libraries(${_testname} KConfigCore KConfigGui Qt5::Test Qt5::Widgets)
      ecm_mark_as_test(${_testname})
   endforeach()
endmacro()

kconfigcore_unit_tests(
   kconfignokdehometest
   kconfigtest
   kdesktopfiletest
   ksharedconfigtest
   ksharedptrtest
)

# compile KEntryMap into the test since it's not exported
set(kentrymaptest_SRCS kentrymaptest.cpp ../src/core/kconfigdata.cpp)
add_executable(kentrymaptest ${kentrymaptest_SRCS})
add_test(kconfigcore-kentrymaptest kentrymaptest)
target_link_libraries(kentrymaptest KConfigCore Qt5::Test)
ecm_mark_as_test(kentrymaptest)

kconfiggui_unit_tests(
   kconfigguitest
   kconfigskeletontest
   kstandardshortcuttest
)
if(WIN32)
    target_link_libraries(kconfigguitest ${KDEWIN_LIBRARIES})
endif()

add_subdirectory(kconfig_compiler)

