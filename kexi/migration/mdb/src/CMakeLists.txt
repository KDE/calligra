if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
    add_definitions(-Wno-unused-but-set-variable)
    add_definitions(-Wno-missing-format-attribute)
    add_definitions(-Wno-sign-compare)
    add_definitions(-Wno-unused-parameter)
endif ()

set(MDBTOOLS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/kexi/migration/mdb/3rdparty/mdbtools")

include_directories(  ${GLIB2_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/kexi  ${MDBTOOLS_SOURCE_DIR}/include )

include(FindIconv)
if(ICONV_FOUND)
    add_definitions(-DHAVE_ICONV=1)
    include_directories(${ICONV_INCLUDE_DIR})
else()
    message(FATAL_ERROR "Could not find library with iconv and iconv headers. Required for MS Access import plugin.")
endif()

########### mdb migrate target ###############

set(keximigrate_mdbmigrate_PART_SRCS keximdb/mdbmigrate.cpp
${MDBTOOLS_SOURCE_DIR}/libmdb/catalog.c
${MDBTOOLS_SOURCE_DIR}/libmdb/data.c
${MDBTOOLS_SOURCE_DIR}/libmdb/dump.c
${MDBTOOLS_SOURCE_DIR}/libmdb/file.c
${MDBTOOLS_SOURCE_DIR}/libmdb/iconv.c
${MDBTOOLS_SOURCE_DIR}/libmdb/index.c
${MDBTOOLS_SOURCE_DIR}/libmdb/like.c
${MDBTOOLS_SOURCE_DIR}/libmdb/map.c
${MDBTOOLS_SOURCE_DIR}/libmdb/mem.c
${MDBTOOLS_SOURCE_DIR}/libmdb/money.c
${MDBTOOLS_SOURCE_DIR}/libmdb/options.c
${MDBTOOLS_SOURCE_DIR}/libmdb/sargs.c
${MDBTOOLS_SOURCE_DIR}/libmdb/table.c
${MDBTOOLS_SOURCE_DIR}/libmdb/worktable.c
${MDBTOOLS_SOURCE_DIR}/libmdb/write.c
)


add_library(keximigrate_mdb MODULE ${keximigrate_mdbmigrate_PART_SRCS})



target_link_libraries(keximigrate_mdb
    ${GLIB2_LIBRARIES}
    ${ICONV_LIBRARIES}
    keximigrate
    kexiutils

    KDb
)

install(TARGETS keximigrate_mdb  DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
