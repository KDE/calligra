
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_program(BASH_PROGRAM bash)
    find_program(XGETTEXT_PROGRAM xgettext)
    find_program(MSGCAT_PROGRAM msgcat)

    if (BASH_PROGRAM AND XGETTEXT_PROGRAM AND MSGCAT_PROGRAM)
        macro(CALLIGRA_ADD_TEST_COMMAND _TEST_NAME)
            add_test(${_TEST_NAME} ${BASH_PROGRAM} "${CMAKE_CURRENT_SOURCE_DIR}/${_TEST_NAME}.sh"
                                    "${CMAKE_SOURCE_DIR}/kundo2_aware_xgettext.sh"
                                    "${XGETTEXT_PROGRAM}"
                                    "${MSGCAT_PROGRAM}"
                                    "${CMAKE_CURRENT_BINARY_DIR}/po"
            )
        endmacro()
        macro(CALLIGRA_ADD_UNIT_TEST _TEST_NAME)
            add_custom_target(${_TEST_NAME} ALL
                COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${_TEST_NAME}.cpp" "${CMAKE_CURRENT_BINARY_DIR}/${_TEST_NAME}.cpp"
                COMMENT "Copy test files" VERBATIM
            )
            CALLIGRA_ADD_TEST_COMMAND(${_TEST_NAME})
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
        CALLIGRA_ADD_UNIT_TEST(test_i18n)
        CALLIGRA_ADD_UNIT_TEST(test_i18nc)
        CALLIGRA_ADD_UNIT_TEST(test_i18nc_long)

        CALLIGRA_ADD_UNIT_TEST(test_i18np)
        CALLIGRA_ADD_UNIT_TEST(test_i18n_noop)
        CALLIGRA_ADD_UNIT_TEST(test_i18n_noop2)

        CALLIGRA_ADD_UNIT_TEST(test_xi18n)
        CALLIGRA_ADD_UNIT_TEST(test_xi18nc)
        CALLIGRA_ADD_UNIT_TEST(test_xi18nc_long)
        CALLIGRA_ADD_UNIT_TEST(test_xi18np)
        CALLIGRA_ADD_UNIT_TEST(test_xi18ncp)

        CALLIGRA_ADD_UNIT_TEST(test_kundo2_i18n)
        CALLIGRA_ADD_UNIT_TEST(test_kundo2_i18nc)
        CALLIGRA_ADD_UNIT_TEST(test_kundo2_i18nc_long)

        # this uses cpp files from earlier tests
        CALLIGRA_ADD_TEST_COMMAND(test_i18n_mixed)

        # add test of Messages.sh
        add_test(test_Messages ${BASH_PROGRAM} "${CMAKE_CURRENT_SOURCE_DIR}/test_messages.sh" "${CMAKE_SOURCE_DIR}")

    endif(BASH_PROGRAM AND XGETTEXT_PROGRAM AND MSGCAT_PROGRAM)

endif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
