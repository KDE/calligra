project( lcmsengine )

if(BUILD_TESTING AND NOT WIN32)
## Only test if on non-Windows system
    add_subdirectory(tests)
endif()

add_subdirectory(data)

include_directories( ${PIGMENT_INCLUDES} ${LCMS2_INCLUDE_DIR} )

include_directories( 
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/cmyk_u8
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/cmyk_u16
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/cmyk_f32
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/gray_u8
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/gray_u16
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/gray_f32
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/lab_u8
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/lab_u16
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/lab_f32
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/rgb_u8
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/rgb_u16
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/rgb_f32
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/xyz_u8
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/xyz_u16
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/xyz_f32
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/ycbcr_u8
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/ycbcr_u16
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/ycbcr_f32
    ${CMAKE_CURRENT_SOURCE_DIR}/colorprofiles
)

if (OpenEXR_FOUND)
include_directories( 
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/gray_f16
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/rgb_f16
    ${CMAKE_CURRENT_SOURCE_DIR}/colorspaces/xyz_f16
)
endif ()

set(FILE_OPENEXR_SOURCES)
set(LINK_OPENEXR_LIB)
if(OpenEXR_FOUND)
    include_directories(${OPENEXR_INCLUDE_DIR})
    set(LINK_OPENEXR_LIB ${OPENEXR_LIBRARIES})
    add_definitions(${OPENEXR_DEFINITIONS})
endif()


set ( lcmsengine_SRCS
    colorspaces/cmyk_u8/CmykU8ColorSpace.cpp
    colorspaces/cmyk_u16/CmykU16ColorSpace.cpp
    colorspaces/cmyk_f32/CmykF32ColorSpace.cpp

    colorspaces/gray_u8/GrayU8ColorSpace.cpp
    colorspaces/gray_u16/GrayU16ColorSpace.cpp
    colorspaces/gray_f32/GrayF32ColorSpace.cpp

    colorspaces/lab_u8/LabU8ColorSpace.cpp
    colorspaces/lab_u16/LabColorSpace.cpp
    colorspaces/lab_f32/LabF32ColorSpace.cpp

    colorspaces/xyz_u8/XyzU8ColorSpace.cpp
    colorspaces/xyz_u16/XyzU16ColorSpace.cpp
    colorspaces/xyz_f32/XyzF32ColorSpace.cpp

    colorspaces/rgb_u8/RgbU8ColorSpace.cpp
    colorspaces/rgb_u16/RgbU16ColorSpace.cpp
    colorspaces/rgb_f32/RgbF32ColorSpace.cpp

    colorspaces/ycbcr_u8/YCbCrU8ColorSpace.cpp
    colorspaces/ycbcr_u16/YCbCrU16ColorSpace.cpp
    colorspaces/ycbcr_f32/YCbCrF32ColorSpace.cpp

    colorprofiles/LcmsColorProfileContainer.cpp
    colorprofiles/IccColorProfile.cpp
    IccColorSpaceEngine.cpp
    LcmsColorSpace.cpp
    LcmsEnginePlugin.cpp
)

if (OpenEXR_FOUND)
    set ( lcmsengine_SRCS
        ${lcmsengine_SRCS}
        colorspaces/gray_f16/GrayF16ColorSpace.cpp
        colorspaces/rgb_f16/RgbF16ColorSpace.cpp
        colorspaces/xyz_f16/XyzF16ColorSpace.cpp
    )
endif ()



add_library(kolcmsengine MODULE ${lcmsengine_SRCS})

target_link_libraries(kolcmsengine pigmentcms kowidgets KF6::I18n  ${LCMS2_LIBRARIES}  ${LINK_OPENEXR_LIB})
install(TARGETS kolcmsengine DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/colorspaces)

