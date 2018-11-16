
find_program(BASH_PROGRAM bash)
find_program(XGETTEXT_PROGRAM xgettext)
find_program(MSGCAT_PROGRAM msgcat)

if (BASH_PROGRAM AND XGETTEXT_PROGRAM AND MSGCAT_PROGRAM)
    find_program(APPLICATION_PROGRAM calligrawords)
    if (APPLICATION_PROGRAM)
        message(STATUS "Found installed 'calligrawords': ${APPLICATION_PROGRAM}")

        macro(ADD_APPLICATION_TEST_COMMAND _TEST_NAME _TEST_FILE)
            add_test(${_TEST_NAME} ${BASH_PROGRAM} "${CMAKE_SOURCE_DIR}/tests/test_installed_application.sh"
                    "${APPLICATION_PROGRAM}" "${CALLIGRA_VERSION_STRING}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/data/${_TEST_FILE}"
                    "${CMAKE_BINARY_DIR}"
            )
        endmacro()

        ADD_APPLICATION_TEST_COMMAND(test-words-allcharts allcharts.odt)
    else()
        message(STATUS "Could not find installed 'calligrawords'")
    endif()
endif()
