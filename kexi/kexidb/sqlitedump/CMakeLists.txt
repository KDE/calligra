set(KEXI_SQLITE_DUMP_TOOL kexi_sqlite3_dump)
add_definitions( -DKEXI_SQLITE_DUMP_TOOL=\"${KEXI_SQLITE_DUMP_TOOL}\" )

set(kexi_sqlite_dump_tool_SRCS main.cpp shell.c)

kde4_add_executable(${KEXI_SQLITE_DUMP_TOOL} NOGUI ${kexi_sqlite_dump_tool_SRCS})

target_link_libraries(${KEXI_SQLITE_DUMP_TOOL} ${SQLITE_LIBRARIES} ${QT_QTCORE_LIBRARY})

install(TARGETS ${KEXI_SQLITE_DUMP_TOOL} ${INSTALL_TARGETS_DEFAULT_ARGS})
