
INCLUDES  = $(all_includes) -I$(idldir)
LDFLAGS =   $(all_libraries)
LDADD =  -lkfile -lkfm -lkdeui -lkdecore -lqt $(LIBMICO) -lXext \
	-lX11 -lkom -lop -lkoml -lkofficecore -lkofficeui

####### Files

bin_PROGRAMS = img

img_SOURCES = img_main.cc img_doc.cc img_view.cc img.cc img_shell.cc
noinst_HEADERS = img_main.h img_doc.h img_view.h img.h img_shell.h
SRCMETA =	img_main.moc img_view.moc img_doc.moc img_shell.moc

idl_DATA = img.idl
idl_HEADER =  img.h

kdelnk_DATA = kimage_part.kdelnk
kdelnkdir = $(kde_datadir)/koffice/partlnk/kimage.kdelnk

####### Implicit rules

SUFFIXES = .h .moc

.h.moc:
	$(MOC) $< -o $@      

####### Build rules

idl: img.idl
	idl --query-server-for-narrow -I$(KDEDIR)/idl img.idl

img_doc.cc: img_doc.moc
img_main.cc: img_main.moc
img_shell.cc: img_shell.moc
img_view.cc: img_view.moc

