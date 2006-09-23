

macro_optional_find_package(PNG)
macro_optional_find_package(OpenEXR)
macro_optional_find_package(TIFF)
macro_optional_find_package(JPEG)
macro_optional_find_package(ImageMagick)

include_directories(${KRITA_INCLUDES})

add_subdirectory( raw )

if(JPEG_FOUND AND LIBEXIF_FOUND)
	add_subdirectory(jpeg)
endif(JPEG_FOUND AND LIBEXIF_FOUND)

if(TIFF_FOUND AND LIBEXIF_FOUND)
	add_subdirectory(tiff)
endif(TIFF_FOUND AND LIBEXIF_FOUND)

message(STATUS "someone needs to port the krita plugins!")
if(PNG_FOUND)
#    add_subdirectory(png)
endif(PNG_FOUND)

message(STATUS "reactivate openexr when colorspaces f32 and f16 are reactivated")
if(OPENEXR_FOUND)
#	add_subdirectory(openexr)
endif(OPENEXR_FOUND)

if(GRAPHICSMAGICK_FOUND)
#	add_subdirectory(magick)
endif(GRAPHICSMAGICK_FOUND)

