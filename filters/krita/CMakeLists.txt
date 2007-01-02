macro_optional_find_package(PNG)
macro_optional_find_package(OpenEXR)
macro_optional_find_package(TIFF)
macro_optional_find_package(JPEG)
macro_optional_find_package(GraphicsMagick)
macro_optional_find_package(PopplerQt4)

include_directories(${KRITA_INCLUDES})

add_subdirectory( raw )

if(JPEG_FOUND AND LIBEXIF_FOUND)
	add_subdirectory(jpeg)
endif(JPEG_FOUND AND LIBEXIF_FOUND)

if(TIFF_FOUND AND LIBEXIF_FOUND)
	add_subdirectory(tiff)
endif(TIFF_FOUND AND LIBEXIF_FOUND)

if(PNG_FOUND)
    add_subdirectory(png)
endif(PNG_FOUND)

if(OPENEXR_FOUND)
    add_subdirectory(openexr)
endif(OPENEXR_FOUND)

if(GRAPHICSMAGICK_FOUND)
    add_subdirectory(gmagick)
endif(GRAPHICSMAGICK_FOUND)

if(POPPLER_QT4_FOUND)
    add_subdirectory(pdf)
endif(POPPLER_QT4_FOUND)
