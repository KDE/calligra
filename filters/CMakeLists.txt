include(CheckIncludeFiles)

macro_optional_find_package(WPD)
macro_log_feature(WPD_FOUND "WPD" "WordPerfect Document Library" "http://libwpd.sourceforge.net/" FALSE "" "Required by the Words WPD import filter")

macro_optional_find_package(WPG)
macro_log_feature(LIBWPG_FOUND "WPG" "WordPerfect Graphics Library" "http://libwpg.sourceforge.net/" FALSE "" "Required by the Karbon WPG import filter")

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

set(libdialogfilter_SRCS 
	${CMAKE_SOURCE_DIR}/filters/libdialogfilter/exportsizedia.cpp
)

set(liboofilter_SRCS 
	${CMAKE_SOURCE_DIR}/filters/liboofilter/ooutils.cc 
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

if(SHOULD_BUILD_FLOW OR SHOULD_BUILD_WORDS_PART)
    if(WPD_FOUND AND LIBWPG_FOUND)
        set(WRITERPERFECT_INCLUDES ${CMAKE_SOURCE_DIR}/filters/writerperfect)
        add_subdirectory( writerperfect )
    endif(WPD_FOUND AND LIBWPG_FOUND)
endif(SHOULD_BUILD_FLOW OR SHOULD_BUILD_WORDS_PART)

set(SHOULD_BUILD_OOXML_FILTERS ${SHARED_MIME_INFO_FOUND})

add_subdirectory(libmso)

if(SHOULD_BUILD_OOXML_FILTERS)
    add_subdirectory( libmsooxml )
endif(SHOULD_BUILD_OOXML_FILTERS)

if(SHOULD_BUILD_KCHART)
    add_subdirectory(kchart)
endif(SHOULD_BUILD_KCHART)

if (SHOULD_BUILD_WORDS_PART)
    add_subdirectory(words)
endif (SHOULD_BUILD_WORDS_PART)

if(SHOULD_BUILD_STAGE_PART)
    add_subdirectory(stage)
endif(SHOULD_BUILD_STAGE_PART)

if(SHOULD_BUILD_SHEETS_PART)
    add_subdirectory(sheets)
endif(SHOULD_BUILD_SHEETS_PART)

if(SHOULD_BUILD_FLOW)
    add_subdirectory(flow)
endif(SHOULD_BUILD_FLOW)

if(SHOULD_BUILD_PLAN)
    add_subdirectory(plan)
endif(SHOULD_BUILD_PLAN)

if(SHOULD_BUILD_KARBON)
  add_subdirectory(karbon)
endif(SHOULD_BUILD_KARBON)

