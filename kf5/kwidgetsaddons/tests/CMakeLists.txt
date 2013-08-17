
include(ECMMarkAsTest)

find_package(Qt5 5.2.0 REQUIRED NO_MODULE COMPONENTS Xml Test)

macro(kwidgetsaddons_executable_tests)
  foreach(_testname ${ARGN})
    add_executable(${_testname} ${_testname}.cpp)
    target_link_libraries(${_testname} Qt5::Widgets Qt5::Test Qt5::Xml KWidgetsAddons)
    ecm_mark_as_test(${_testname})
  endforeach(_testname)
endmacro()

kwidgetsaddons_executable_tests(
  kledtest
  kmessageboxtest
  kselectactiontest
  kseparatortest
  ksqueezedtextlabeltest
  ktitlewidgettest
  kfontrequestertest
  kpassworddialogtest
  keditlistwidgettest
  kratingwidgettest
  kactionselectortest
  kcolorcombotest
  kpixmapregionselectordialogtest
  kpassivepopuptest
  ktoolbarlabelactiontest
  kpagedialogtest
  kpagewidgettest
  kassistantdialogtest
  kmessagewidgettest
  knewpassworddialogtest
)
