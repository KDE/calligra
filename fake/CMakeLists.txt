find_package(ZLIB)
set_package_properties(ZLIB PROPERTIES DESCRIPTION "Support for gzip compressed files and data streams"
                       URL "http://www.zlib.net"
                       TYPE REQUIRED
                       PURPOSE "Required by the core KDE libraries and some critical kioslaves"
                      )


set(kofake_LIB_SRCS
    karchive.cpp
    kcomponentdata.cpp
    kconfigbase.cpp
    kfilterbase.cpp
    kfilterdev.cpp
    kgzipfilter.cpp
    klimitediodevice.cpp
    kpluginfactory.cpp
    ktar.cpp
    kzip.cpp
    kio/job.cpp
)

kde4_add_library(kofake SHARED ${kofake_LIB_SRCS})

target_link_libraries(kofake Qt5::Widgets ${ZLIB_LIBRARIES})
target_link_libraries(kofake LINK_INTERFACE_LIBRARIES Qt5::Widgets)

set_target_properties(kofake PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS kofake ${INSTALL_TARGETS_DEFAULT_ARGS} )

