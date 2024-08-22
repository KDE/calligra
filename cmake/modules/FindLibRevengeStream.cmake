# SPDX-FileCopyrightText: 2024 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-3-Clause

find_package(PkgConfig QUIET)
pkg_check_modules(PKG_LibRevengeStream QUIET librevenge-stream-0.0 IMPORTED_TARGET)

set(LibRevengeStream_VERSION ${PKG_LibRevengeStream_VERSION})
set(LibRevengeStream_PREFIX ${PKG_LibRevengeStream_PREFIX})

find_path(LibRevengeStream_INCLUDE_DIRS
    NAMES librevenge-stream/librevenge-stream.h
    HINTS ${PKG_LibRevengeStream_INCLUDE_DIRS}
)

find_library(LibRevengeStream_LIBRARIES
    NAMES revenge-stream librevenge-stream revenge-stream-0.0 librevenge-stream-0.0
    HINTS ${PKG_LibRevengeStream_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibRevengeStream
    FOUND_VAR LibRevengeStream_FOUND
    REQUIRED_VARS LibRevengeStream_PREFIX LibRevengeStream_INCLUDE_DIRS
    VERSION_VAR LibRevengeStream_VERSION
)

if(LibRevengeStream_FOUND AND NOT TARGET LibRevenge::LibRevengeStream)
    add_library(LibRevenge::LibRevengeStream UNKNOWN IMPORTED)
    set_target_properties(LibRevenge::LibRevengeStream PROPERTIES
        IMPORTED_LOCATION "${LibRevengeStream_LIBRARIES}"
        INTERFACE_COMPILE_OPTIONS "${PKG_LibRevengeStream_CFLAGS}"
        INTERFACE_INCLUDE_DIRECTORIES "${LibRevengeStream_INCLUDE_DIRS}"
    )
    if (TARGET PkgConfig::PKG_LibRevengeStream)
        target_link_libraries(LibRevenge::LibRevengeStream INTERFACE PkgConfig::PKG_LibRevengeStream)
    endif()
endif()

mark_as_advanced(LibRevengeStream_LIBRARIES LibRevengeStream_INCLUDE_DIRS LibRevengeStream_VERSION)

include(FeatureSummary)
set_package_properties(LibRevengeStream PROPERTIES
    URL "http://sf.net/p/libwpd/librevenge/"
    DESCRIPTION "Sample Input stream implementation for librevenge"
)
