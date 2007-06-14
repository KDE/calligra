include_directories(${KRITA_INCLUDES} ${CMAKE_SOURCE_DIR}/krita/image/metadata)

set(libkisexiv2_LIB_SRCS
    ${CMAKE_SOURCE_DIR}/filters/krita/libkisexiv2/kis_exif_io.cpp
    ${CMAKE_SOURCE_DIR}/filters/krita/libkisexiv2/kis_iptc_io.cpp
    ${CMAKE_SOURCE_DIR}/filters/krita/libkisexiv2/kis_exiv2.cpp
    )

kde4_add_library(kisexiv2 SHARED ${libkisexiv2_LIB_SRCS})

target_link_libraries(kisexiv2 kritaimage exiv2)
install(TARGETS kisexiv2 DESTINATION ${LIB_INSTALL_DIR})
