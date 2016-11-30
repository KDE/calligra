
find_program(BASH_PROGRAM bash)
find_program(XGETTEXT_PROGRAM xgettext)
find_program(MSGCAT_PROGRAM msgcat)

if (BASH_PROGRAM AND XGETTEXT_PROGRAM AND MSGCAT_PROGRAM)
    macro(CALLIGRA_ADD_UNIT_TEST _TEST_NAME)
        add_custom_target(${_TEST_NAME} ALL
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${_TEST_NAME}.cpp" "${CMAKE_CURRENT_BINARY_DIR}/${_TEST_NAME}.cpp"
            COMMENT "Copy test files" VERBATIM
        )
        add_test(${_TEST_NAME} ${BASH_PROGRAM} "${CMAKE_CURRENT_SOURCE_DIR}/${_TEST_NAME}.sh" 
                                "${CMAKE_SOURCE_DIR}/calligra_xgettext.sh"
                                "${XGETTEXT_PROGRAM}"
                                "${MSGCAT_PROGRAM}"
                                "${CMAKE_CURRENT_BINARY_DIR}/po"
        )
    endmacro()

    # create the podir
    add_custom_target(podir ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/po
        COMMENT "Create podir" VERBATIM)

    # add tests here
    CALLIGRA_ADD_UNIT_TEST(test_i18n)
    CALLIGRA_ADD_UNIT_TEST(test_kundo2_i18n)
    CALLIGRA_ADD_UNIT_TEST(test_kundo2_i18nc_long)

endif()
