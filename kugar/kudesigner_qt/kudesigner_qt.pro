SOURCES	+= canvbox.cpp \
	ccalcfield.cpp \
	cfield.cpp \
	clabel.cpp \
	cline.cpp \
	creportitem.cpp \
	cspecialfield.cpp \
	cv.cpp \
	itemopts.cpp \
	main.cpp \
	mycanvas.cpp
HEADERS	+= canvbox.h \
	canvdefs.h \
	ccalcfield.h \
	cfield.h \
	clabel.h \
	cline.h \
	creportitem.h \
	cspecialfield.h \
	cv.h \
	dlgnew.ui.h \
	itemopts.h \
	mainform.ui.h \
	mycanvas.h

MAKEFILE=Makefile.qt
TARGET = kudesigner 
OBJECTS_DIR = obj 
UI_DIR = ui 
MOC_DIR = moc 
FORMS	= dlgnew.ui \
	dlgoptions.ui \
	mainform.ui
IMAGES	= images/fileopen \
	images/filesave \
	images/print \
	images/undo \
	images/redo \
	images/editcut \
	images/editcopy \
	images/editpaste \
	images/searchfind \
	images/font_names.png \
	images/frame_text.png \
	images/insert_table_col.png \
	images/frame_chart.png \
	images/frame_edit.png \
	images/frame_formula.png \
	images/frame_image.png \
	images/frame_query.png \
	images/frame_text.png \
	images/insert_table_col.png \
	images/insert_table_row.png \
	images/i-regular-36-aa.png \
	images/arrow.png \
	images/frame_edit.png \
	images/frame_field.png \
	images/i-regular-36-bb.png \
	images/i-d.png \
	images/i-df.png \
	images/i-dh.png \
	images/i-pf.png \
	images/i-ph.png \
	images/i-rh.png \
	images/i-rf.png \
	
TEMPLATE	=app
CONFIG	+= release warn_on thread
LANGUAGE	= C++
