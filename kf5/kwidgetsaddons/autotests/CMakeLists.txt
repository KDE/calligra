
include(ECMMarkAsTest)

find_package(Qt5 5.2.0 REQUIRED NO_MODULE COMPONENTS Xml Test)

macro(kwidgetsaddons_unit_tests)
  foreach(_testname ${ARGN})
    add_executable(${_testname} ${_testname}.cpp)
    target_link_libraries(${_testname} Qt5::Widgets Qt5::Test Qt5::Xml KWidgetsAddons)
    add_test(kwidgetsaddons-${_testname} ${_testname})
    ecm_mark_as_test(${_testname})
  endforeach(_testname)
endmacro()

kwidgetsaddons_unit_tests(
  kcolorbuttontest
  kdualactiontest
  kselectaction_unittest
)
