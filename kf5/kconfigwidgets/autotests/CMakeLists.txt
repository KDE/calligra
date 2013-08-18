
find_package(Qt5 5.2.0 REQUIRED NO_MODULE COMPONENTS Xml Test)

include(ECMMarkAsTest)

macro(KCONFIGWIDGETS_TESTS)
    foreach(_testname ${ARGN})
        add_executable(${_testname} ${_testname}.cpp)
        add_test(kconfigwidgets-${_testname} ${_testname})
        ecm_mark_as_test(${_testname})
        target_link_libraries(${_testname} Qt5::Widgets Qt5::Test Qt5::Xml KConfigWidgets KWidgetsAddons)
    endforeach(_testname)
endmacro()

kconfigwidgets_tests(
  kconfigdialog_unittest
  kstandardactiontest
)
