
include(ECMMarkAsTest)

find_package(Qt5 5.2.0 REQUIRED NO_MODULE COMPONENTS Xml Test)

macro(kconfigwidgets_executable_tests)
  foreach(_testname ${ARGN})
    add_executable(${_testname} ${_testname}.cpp)
    target_link_libraries(${_testname} Qt5::Test Qt5::Xml KConfigWidgets)
    ecm_mark_as_test(${_testname})
  endforeach(_testname)
endmacro()

kconfigwidgets_executable_tests(
   kcodecactiontest
   klanguagebuttontest
)

## kcolorutilsdemo

set(kcolorUtilsDemoSources kcolorutilsdemo.cpp kimageframe.cpp)
qt5_wrap_ui(kcolorUtilsDemoSources kcolorutilsdemo.ui)
add_executable(kcolorutilsdemo ${kcolorUtilsDemoSources})
ecm_mark_as_test(kcolorutilsdemo)
target_link_libraries(kcolorutilsdemo KConfigWidgets KGuiAddons)
