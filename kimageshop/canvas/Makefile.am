
INCLUDES  = $(MICO_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes)
LDFLAGS   = $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD     = $(LIB_KOFFICEUI) $(LIB_KIMGIO) -lqimgio -ljpeg

#SUBDIRS = icons images

bin_PROGRAMS = canvas testFastPixmap

canvas_SOURCES = brush.cc canvas.cc channel.cc layer.cc layerlist.cc misc.cc numSlider.cc main.cc

testFastPixmap_SOURCES = testFastPixmap.cc

noinst_HEADERS = brush.h canvas.h channel.h layer.h layerlist.h misc.h numSlider.h

METASOURCES = canvas.moc layer.moc numSlider.moc

