set(status_qrc "${CMAKE_CURRENT_SOURCE_DIR}/status.qrc")
set(status_rcc "${CMAKE_CURRENT_BINARY_DIR}/status.rcc")

add_custom_command(OUTPUT "${status_rcc}"
    COMMAND "${QT_RCC_EXECUTABLE}" -binary -o "${status_rcc}" "${status_qrc}"
    DEPENDS "${status_qrc}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Building external Qt resource ${status_rcc}"
    VERBATIM)

add_custom_target(build_status_rcc DEPENDS "${status_rcc}" SOURCES "${status_qrc}")

add_dependencies(keximain build_status_rcc)

install(FILES
        status.ui
        contribution_details.ui
        contribution_help.ui
        status_strings.ui
        ${status_rcc}
        DESTINATION "${DATA_INSTALL_DIR}/kexi/status/2.5.0")
