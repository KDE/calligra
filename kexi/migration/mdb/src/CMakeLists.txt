
include_directories(  ${GLIB2_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/kexi ${KDE4_INCLUDES} ${CMAKE_SOURCE_DIR}/kexi/migration/mdb/src/mdbtools/include )

INCLUDE(FindIconv)
if(ICONV_FOUND)
    add_definitions(-DHAVE_ICONV=1)
    include_directories(${ICONV_INCLUDE_DIR})
else(ICONV_FOUND)
    message(FATAL_ERROR "Could not find library with iconv and iconv headers. Required for MS Access import plugin.")
endif(ICONV_FOUND)

########### mdb migrate target ###############

set(keximigrate_mdbmigrate_PART_SRCS keximdb/mdbmigrate.cpp
mdbtools/libmdb/catalog.c
mdbtools/libmdb/data.c
mdbtools/libmdb/dump.c
mdbtools/libmdb/file.c
mdbtools/libmdb/iconv.c
mdbtools/libmdb/index.c
mdbtools/libmdb/like.c
mdbtools/libmdb/map.c
mdbtools/libmdb/mem.c
mdbtools/libmdb/money.c
mdbtools/libmdb/options.c
mdbtools/libmdb/sargs.c
mdbtools/libmdb/table.c
mdbtools/libmdb/worktable.c
mdbtools/libmdb/write.c
)


kde4_add_plugin(keximigrate_mdb ${keximigrate_mdbmigrate_PART_SRCS})



target_link_libraries(keximigrate_mdb
${KDE4_KPARTS_LIBS}
${GLIB2_LIBRARIES}
${ICONV_LIBRARIES}
keximigrate
kexidb
kexiutils
)

install(TARGETS keximigrate_mdb  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  keximdb/keximigrate_mdb.desktop  DESTINATION ${SERVICES_INSTALL_DIR})