set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( .. ${KPLATO_INCLUDES} ${KOODF_INCLUDES})

# call: plan_add_unit_test(<test-name> <sources> LINK_LIBRARIES <library> [<library> [...]] [GUI])
macro(PLAN_ADD_UNIT_TEST _TEST_NAME)
    ecm_add_test( ${ARGN}
        TEST_NAME "${_TEST_NAME}"
        NAME_PREFIX "plan-"
    )
endmacro()

########### test files ###############

set(test_files version-0-6.kplato project_stats1.kplato pert1.kplato)
foreach(it ${test_files})
    add_custom_target(${it} ALL
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${it} ${CMAKE_CURRENT_BINARY_DIR}/${it}
        COMMENT "Copy test file ${it}" VERBATIM
    )
endforeach(it)

########### next target ###############

# NOTE: .kplato files is no longer native, so need some magic to run this test 
# plan_add_unit_test(InsertFileTester
#     InsertFileTester.cpp
#     LINK_LIBRARIES planprivate kplatokernel komain Qt5::Test
# )

########## next target ###############

plan_add_unit_test(InsertProjectTester
    InsertProjectTester.cpp
    LINK_LIBRARIES planprivate kplatokernel komain Qt5::Test
)
