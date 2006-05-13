

macro_optional_find_package(PNG)
macro_optional_find_package(OpenEXR)
macro_optional_find_package(TIFF)
macro_optional_find_package(JPEG)
macro_optional_find_package(ImageMagick)

include_directories(${KRITA_INCLUDES})

add_subdirectory( raw )

if(JPEG_FOUND)
	add_subdirectory(jpeg)
endif(JPEG_FOUND)

if(TIFF_FOUND)
	add_subdirectory(tiff)
endif(TIFF_FOUND)		

if(PNG_FOUND)
    add_subdirectory(png)
endif(PNG_FOUND)

if(OPENEXR_FOUND)
	add_subdirectory(openexr)
endif(OPENEXR_FOUND)

if(IMAGEMAGICK_FOUND)
	add_subdirectory(magick)
endif(IMAGEMAGICK_FOUND)



#original Makefile.am contents follow:

#if have_openexr
#OPENEXR_SUBDIR=openexr
#endif
#
#if include_imagemagick_filter
#IMAGEMAGICK_SUBDIR=magick
#endif
#
#if have_png
#PNG_SUBDIR=png
#endif
#
#if include_jpeg_filter
#JPEG_SUBDIR=jpeg
#endif
#
#if include_tiff_filter
#TIFF_SUBDIR=tiff
#endif
#
#if have_exif
#LIBKISEXIF=libkisexif
#endif
#
#SUBDIRS = $(IMAGEMAGICK_SUBDIR) $(OPENEXR_SUBDIR) $(PNG_SUBDIR) $(LIBKISEXIF) \
#	$(JPEG_SUBDIR) $(TIFF_SUBDIR) raw 
