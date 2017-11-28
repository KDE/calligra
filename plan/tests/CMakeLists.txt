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
    LINK_LIBRARIES planprivate kplatokernel planmain Qt5::Test
)

###### Add i18n unit tests #############

find_program(BASH_PROGRAM bash)
find_program(XGETTEXT_PROGRAM xgettext)
find_program(MSGCAT_PROGRAM msgcat)

if (BASH_PROGRAM AND XGETTEXT_PROGRAM AND MSGCAT_PROGRAM)
    macro(PLAN_ADD_TEST_COMMAND _TEST_NAME)
        add_test("plan-${_TEST_NAME}" ${BASH_PROGRAM} "${CMAKE_CURRENT_SOURCE_DIR}/${_TEST_NAME}.sh"
                                "${PLAN_SOURCE_DIR}/kundo2_aware_xgettext.sh"
                                "${XGETTEXT_PROGRAM}"
                                "${MSGCAT_PROGRAM}"
                                "${CMAKE_CURRENT_BINARY_DIR}/po"
        )
    endmacro()
    macro(PLAN_ADD_UNIT_TEST _TEST_NAME)
        add_custom_target(${_TEST_NAME} ALL
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${_TEST_NAME}.cpp" "${CMAKE_CURRENT_BINARY_DIR}/${_TEST_NAME}.cpp"
            COMMENT "Copy test files" VERBATIM
        )
        PLAN_ADD_TEST_COMMAND(${_TEST_NAME})
    endmacro()

    # create the podir
    add_custom_target(podir ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/po
        COMMENT "Create podir" VERBATIM)
    # copy parameters script
    add_custom_target(parameters ALL
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/parameters.sh ${CMAKE_CURRENT_BINARY_DIR}/parameters.sh
        COMMENT "Copy parameters script" VERBATIM)

    # add tests here
    PLAN_ADD_UNIT_TEST(test_i18n)
    PLAN_ADD_UNIT_TEST(test_i18nc)
    PLAN_ADD_UNIT_TEST(test_i18np)
    PLAN_ADD_UNIT_TEST(test_i18nc_long)
    PLAN_ADD_UNIT_TEST(test_i18n_noop)
    PLAN_ADD_UNIT_TEST(test_i18n_noop2)

    PLAN_ADD_UNIT_TEST(test_xi18n)
    PLAN_ADD_UNIT_TEST(test_xi18nc)
    PLAN_ADD_UNIT_TEST(test_xi18nc_long)
    PLAN_ADD_UNIT_TEST(test_xi18np)
    PLAN_ADD_UNIT_TEST(test_xi18ncp)

    PLAN_ADD_UNIT_TEST(test_kundo2_i18n)
    PLAN_ADD_UNIT_TEST(test_kundo2_i18nc)
    PLAN_ADD_UNIT_TEST(test_kundo2_i18nc_long)

    # this uses cpp files from earlier tests
    PLAN_ADD_TEST_COMMAND(test_i18n_mixed)

    # add test of Messages.sh
    add_test(test_Messages ${BASH_PROGRAM} "${CMAKE_CURRENT_SOURCE_DIR}/test_messages.sh" "${PLAN_SOURCE_DIR}")
endif()
