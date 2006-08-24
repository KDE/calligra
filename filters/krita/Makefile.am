if have_openexr
OPENEXR_SUBDIR=openexr
endif

if include_imagemagick_filter
IMAGEMAGICK_SUBDIR=magick
endif

if include_graphicsmagick_filter
GRAPHICSMAGICK_SUBDIR=gmagick
endif

if have_png
PNG_SUBDIR=png
endif

if include_jpeg_filter
JPEG_SUBDIR=jpeg
endif

if include_tiff_filter
TIFF_SUBDIR=tiff
endif

if have_exif
LIBKISEXIF=libkisexif
endif

if include_PDF
PDF_SUBDIR=pdf
endif

SUBDIRS = $(IMAGEMAGICK_SUBDIR) $(OPENEXR_SUBDIR) $(PNG_SUBDIR) $(LIBKISEXIF) \
		$(JPEG_SUBDIR) $(TIFF_SUBDIR) raw $(PDF_SUBDIR) $(GRAPHICSMAGICK_SUBDIR)
