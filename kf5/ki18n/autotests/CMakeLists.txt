remove_definitions(-DQT_NO_CAST_FROM_ASCII)

include(ECMMarkAsTest)

macro(KI18N_UNIT_TESTS)
    foreach(_testname ${ARGN})
        add_executable(${_testname} ${_testname}.cpp)
        target_link_libraries(${_testname} Qt5::Test Qt5::Concurrent KI18n)
        add_test(ki18n-${_testname} ${_testname})
        ecm_mark_as_test(${_testname})
    endforeach()
endmacro()

ki18n_unit_tests(
    klocalizedstringtest
)

