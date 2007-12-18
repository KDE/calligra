include_directories(${KRITA_INCLUDES} ${CMAKE_SOURCE_DIR}/krita/image/metadata ${EXIV2_INCLUDE_DIR})

set(libkisexiv2_LIB_SRCS
    ${CMAKE_SOURCE_DIR}/filters/krita/libkisexiv2/kis_exif_io.cpp
    ${CMAKE_SOURCE_DIR}/filters/krita/libkisexiv2/kis_iptc_io.cpp
    ${CMAKE_SOURCE_DIR}/filters/krita/libkisexiv2/kis_exiv2.cpp
    )

kde4_add_library(kisexiv2 SHARED ${libkisexiv2_LIB_SRCS})

target_link_libraries(kisexiv2 kritaimage ${EXIV2_LIBRARIES})
set_target_properties(kisexiv2 PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kisexiv2 DESTINATION ${LIB_INSTALL_DIR})
