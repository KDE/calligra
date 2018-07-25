set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

## Some tests do not work on freeBSD, so disable these tests until we found out why
STRING (REGEX MATCH "BSD" OS_BSD ${CMAKE_SYSTEM_NAME})
if (OS_BSD)
    message (WARNING "OS is ${CMAKE_SYSTEM_NAME}. Some i18n tests are disabled.")
endif()

###### Add i18n unit tests #############

find_program(BASH_PROGRAM bash)
find_program(XGETTEXT_PROGRAM xgettext)
find_program(MSGCAT_PROGRAM msgcat)

if (BASH_PROGRAM AND XGETTEXT_PROGRAM AND MSGCAT_PROGRAM)
    macro(PLAN_ADD_TEST_COMMAND _TEST_NAME)
        add_test("plan-${_TEST_NAME}" ${BASH_PROGRAM} "${CMAKE_CURRENT_SOURCE_DIR}/${_TEST_NAME}.sh"
                                "${PROJECT_SOURCE_DIR}/kundo2_aware_xgettext.sh"
                                "${XGETTEXT_PROGRAM}"
                                "${MSGCAT_PROGRAM}"
                                "${CMAKE_CURRENT_BINARY_DIR}/po"
        )
    endmacro()
    macro(PLAN_ADD_UNIT_TEST _TEST_NAME)
        add_custom_target(Plan${_TEST_NAME} ALL
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${_TEST_NAME}.cpp" "${CMAKE_CURRENT_BINARY_DIR}/${_TEST_NAME}.cpp"
            COMMENT "Copy test files" VERBATIM
        )
        PLAN_ADD_TEST_COMMAND(${_TEST_NAME})
    endmacro()

    # create the podir
    add_custom_target(Planpodir ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/po
        COMMENT "Create podir" VERBATIM)
    # copy parameters script
    add_custom_target(Planparameters ALL
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/parameters.sh ${CMAKE_CURRENT_BINARY_DIR}/parameters.sh
        COMMENT "Copy parameters script" VERBATIM)

    # add tests here
    if (NOT OS_BSD)
        PLAN_ADD_UNIT_TEST(test_i18n)
        PLAN_ADD_UNIT_TEST(test_i18nc)
        PLAN_ADD_UNIT_TEST(test_i18nc_long)
    endif()

    PLAN_ADD_UNIT_TEST(test_i18np)
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
    if (NOT OS_BSD)
        PLAN_ADD_TEST_COMMAND(test_i18n_mixed)
    endif()

    # add test of Messages.sh
    add_test(test_Messages ${BASH_PROGRAM} "${CMAKE_CURRENT_SOURCE_DIR}/test_messages.sh" "${PROJECT_SOURCE_DIR}")
endif()
