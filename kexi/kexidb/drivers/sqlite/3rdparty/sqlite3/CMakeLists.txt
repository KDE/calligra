
ADD_DEFINITIONS(-DSQLITE_SECURE_DELETE)

include(FindThreads)

set(ksqlite_SRCS sqlite3.c shell.c )
add_executable(ksqlite ${ksqlite_SRCS})
target_link_libraries(ksqlite ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS} ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS ksqlite DESTINATION ${BIN_INSTALL_DIR})
