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

set(liboofilter_SRCS 
	${CMAKE_SOURCE_DIR}/filters/liboofilter/ooutils.cc 
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

if(SHOULD_BUILD_LIB_WRITERPERFECT)
    add_subdirectory( writerperfect )
endif(SHOULD_BUILD_LIB_WRITERPERFECT)

if(SHOULD_BUILD_LIB_ODFTRAVERSE)
    add_subdirectory(libodftraverse)
endif(SHOULD_BUILD_LIB_ODFTRAVERSE)

if(SHOULD_BUILD_LIB_MSO)
    add_subdirectory(libmso)
endif(SHOULD_BUILD_LIB_MSO)

if(SHOULD_BUILD_LIB_MSOOXML)
    add_subdirectory( libmsooxml )
endif(SHOULD_BUILD_LIB_MSOOXML)

if (SHOULD_BUILD_WORDS_PART)
    add_subdirectory(words)
endif (SHOULD_BUILD_WORDS_PART)

if(SHOULD_BUILD_STAGE_PART)
    add_subdirectory(stage)
endif(SHOULD_BUILD_STAGE_PART)

add_subdirectory(sheets)

add_subdirectory(flow)

add_subdirectory(plan)

if(SHOULD_BUILD_KARBON)
  add_subdirectory(karbon)
endif(SHOULD_BUILD_KARBON)

