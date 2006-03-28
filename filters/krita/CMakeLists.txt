kde4_header()

add_subdirectory( raw )

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(IMAGEMAGICK_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(OPENEXR_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(PNG_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(LIBKISEXIF)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(JPEG_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(TIFF_SUBDIR)")



########### install files ###############


kde4_footer()



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
