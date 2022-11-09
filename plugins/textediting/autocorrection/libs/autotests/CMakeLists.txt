# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_definitions( -DAUTOCORRECTION_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )

macro(add_pimcommon_autocorrect_unittest _source)
  set(_test ${_source})
  get_filename_component(_name ${_source} NAME_WE)
  add_executable( ${_name} ${_test} ${_name}.h)
  add_test(NAME ${_name} COMMAND ${_name} )
  set_tests_properties(${_name} PROPERTIES ENVIRONMENT "QT_HASH_SEED=0")
  ecm_mark_as_test(pimcommon-autocorrection-${_name})
  target_link_libraries(${_name}  Qt::Test KF5::PimCommon KF5::PimTextEdit)
endmacro()

add_pimcommon_autocorrect_unittest(autocorrectiontest.cpp)
add_pimcommon_autocorrect_unittest(lineeditwithautocorrectiontest.cpp)
add_pimcommon_autocorrect_unittest(richtexteditwithautocorrectiontest.cpp)
add_pimcommon_autocorrect_unittest(importkmailautocorrectiontest.cpp)
