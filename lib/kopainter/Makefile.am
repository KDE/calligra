SUBDIRS = support . tests

INCLUDES= $(KOFFICEUI_INCLUDES) $(all_includes)

lib_LTLIBRARIES = libkopainter.la
libkopainter_la_SOURCES = koColor.cc \
	koDash.cc \
	koOutline.cc \
	koFill.cc \
	koVectorPath.cc \
	koPainter.cc \
	koFrameButton.cc \
	koColorSlider.cc \
	koColorChooser.cc \
	koIconChooser.cc

libkopainter_la_LDFLAGS = $(all_libraries) -version-info 1:0:0 -no-undefined

libkopainter_la_LIBADD = $(LIB_KOFFICEUI) support/libsupport.la

METASOURCES = AUTO
