project(kritafilters)

macro_optional_find_package(PNG)
macro_optional_find_package(OpenEXR)
macro_optional_find_package(TIFF)
macro_optional_find_package(Exiv2)
macro_optional_find_package(JPEG)
macro_optional_find_package(GraphicsMagick)
macro_optional_find_package(Poppler)

macro_log_feature(JPEG_FOUND "jpeg" "The Krita JPEG filter will not be built" "http://www.ijg.org" FALSE "")
macro_log_feature(EXIV2_FOUND "Exiv2" "The Krita JPEG filter will not be built" "http://www.exiv2.org" FALSE "")
macro_log_feature(TIFF_FOUND "tiff" "The Krita TIFF filter will not be built" "http://www.remotesensing.org/libtiff" FALSE "")
macro_log_feature(PNG_FOUND "png" "The Krita PNG filter will not be built" "http://www.libpng.org/pub/png/libpng.html" FALSE "")
macro_log_feature(OPENEXR_FOUND "OpenEXR" "The Krita OpenEXR filter will not be built" "http://www.openexr.com" FALSE "")
macro_log_feature(GRAPHICSMAGICK_FOUND "GraphicsMagick" "The Krita GraphicsMagick filter for various file formats (including PSD, XCF) will not be built" "http://www.graphicsmagick.org" FALSE "")
macro_log_feature( POPPLER_FOUND "poppler-qt4" "The Krita PDF filter will not be built (version 0.6.x is currently required)" "http://poppler.freedesktop.org" FALSE "")

include_directories(${KRITA_INCLUDES})

message("Krita: Port RAW filter to libkdcraw")
#add_subdirectory( raw )

if(EXIV2_FOUND)
    add_subdirectory(libkisexiv2)
endif(EXIV2_FOUND)

if(JPEG_FOUND AND EXIV2_FOUND)
	add_subdirectory(jpeg)
endif(JPEG_FOUND AND EXIV2_FOUND)

if(TIFF_FOUND)
	add_subdirectory(tiff)
endif(TIFF_FOUND )

if(PNG_FOUND)
    add_subdirectory(png)
endif(PNG_FOUND)

if(OPENEXR_FOUND)
    add_subdirectory(openexr)
endif(OPENEXR_FOUND)

if(GRAPHICSMAGICK_FOUND)
#    add_subdirectory(gmagick)
endif(GRAPHICSMAGICK_FOUND)

if(POPPLER_FOUND AND HAVE_POPPLER_0_6)
    add_subdirectory(pdf)
endif(POPPLER_FOUND AND HAVE_POPPLER_0_6)

add_subdirectory(bmp)
add_subdirectory(ora)
