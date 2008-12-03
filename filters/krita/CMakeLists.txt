project(kritafilters)

macro_optional_find_package(PNG)
macro_optional_find_package(OpenEXR)
macro_optional_find_package(TIFF)
macro_optional_find_package(JPEG)
macro_optional_find_package(Poppler)
macro_optional_find_package(Kdcraw)

macro_optional_find_package(GraphicsMagick)
# if(GRAPHICSMAGICK_FOUND)
#    if(GRAPHICSMAGICK_MAJOR_VERSION GREATER 1 OR GRAPHICSMAGICK_MINOR_VERSION GREATER 1)
#       message(STATUS "GraphicsMagick version ${GRAPHICSMAGICK_VERSION} too recent.  Only 1.1 series supported.")
#       set(GRAPHICSMAGICK_FOUND FALSE)
#    endif(GRAPHICSMAGICK_MAJOR_VERSION GREATER 1 OR GRAPHICSMAGICK_MINOR_VERSION GREATER 1)
#    if(GRAPHICSMAGICK_MAJOR_VERSION LESS 1 OR GRAPHICSMAGICK_MINOR_VERSION LESS 1)
#       message(STATUS "GraphicsMagick version ${GRAPHICSMAGICK_VERSION} too old.  Only 1.1 series supported.")
#       set(GRAPHICSMAGICK_FOUND FALSE)
#    endif(GRAPHICSMAGICK_MAJOR_VERSION LESS 1 OR GRAPHICSMAGICK_MINOR_VERSION LESS 1)
# endif(GRAPHICSMAGICK_FOUND)

macro_log_feature(JPEG_FOUND "jpeg" "The Krita JPEG filter will not be built" "http://www.ijg.org" FALSE "")
macro_log_feature(TIFF_FOUND "tiff" "The Krita TIFF filter will not be built" "http://www.remotesensing.org/libtiff" FALSE "")
macro_log_feature(PNG_FOUND "png" "The Krita PNG filter will not be built" "http://www.libpng.org/pub/png/libpng.html" FALSE "")
macro_log_feature(OPENEXR_FOUND "OpenEXR" "The Krita OpenEXR filter will not be built" "http://www.openexr.com" FALSE "")
macro_log_feature(GRAPHICSMAGICK_FOUND "GraphicsMagick" "The Krita GraphicsMagick filter for various file formats (including PSD, XCF) will not be built" "http://www.graphicsmagick.org" FALSE "1.1" )
macro_log_feature( POPPLER_FOUND "poppler-qt4" "The Krita PDF filter will not be built (version 0.6.x is currently required)" "http://poppler.freedesktop.org" FALSE "")

include_directories(${KRITA_INCLUDES})

message("Krita: Port RAW filter to libkdcraw")
if(KDCRAW_FOUND)
    add_subdirectory( raw )
endif(KDCRAW_FOUND)

if(JPEG_FOUND)
	add_subdirectory(jpeg)
endif(JPEG_FOUND)

if(TIFF_FOUND)
	add_subdirectory(tiff)
endif(TIFF_FOUND )

if(PNG_FOUND)
    add_subdirectory(png)
endif(PNG_FOUND)

# OpenCTL is needed for having a RGB16/32f color space
if(OPENEXR_FOUND AND OPENCTL_FOUND)
    add_subdirectory(openexr)
endif(OPENEXR_FOUND AND OPENCTL_FOUND)

if(GRAPHICSMAGICK_FOUND)
    add_subdirectory(gmagick)
endif(GRAPHICSMAGICK_FOUND)

if(POPPLER_FOUND AND HAVE_POPPLER_0_6)
    add_subdirectory(pdf)
endif(POPPLER_FOUND AND HAVE_POPPLER_0_6)

add_subdirectory(bmp)
add_subdirectory(ora)
