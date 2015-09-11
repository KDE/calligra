add_definitions(-DTRANSLATION_DOMAIN=\"calligrafilters\")

include(CheckIncludeFiles)

# for xpdf:
check_include_files(ndir.h HAVE_NDIR_H)
check_include_files(sys/dir.h HAVE_SYS_DIR_H)
check_include_files(sys/ndir.h HAVE_SYS_NDIR_H)
check_include_files(dirent.h HAVE_DIRENT_H)
# others:
check_include_files(ieeefp.h HAVE_IEEEFP_H)
check_include_files(dirent.h HAVE_DIRENT_H)
check_include_files(string.h HAVE_STRING_H)
check_include_files(strings.h HAVE_STRINGS_H)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-filters.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-filters.h )

kde_enable_exceptions()

if(SHOULD_BUILD_LIB_KOODF2)
    add_subdirectory( libodf2 )
endif()

if(SHOULD_BUILD_LIB_KOODFREADER)
    add_subdirectory(libodfreader)
endif()

if(SHOULD_BUILD_LIB_MSO)
    add_subdirectory(libmso)
endif()

if(SHOULD_BUILD_LIB_KOMSOOXML)
    add_subdirectory( libmsooxml )
endif()

add_subdirectory(words)
add_subdirectory(stage)
add_subdirectory(sheets)
add_subdirectory(flow)
add_subdirectory(karbon)
