

include_directories( ${CMAKE_SOURCE_DIR}/kivio/kiviopart ${CMAKE_SOURCE_DIR}/kivio/kiviopart/kiviosdk ${CMAKE_SOURCE_DIR}/kivio/kiviopart/config ${KDE4_INCLUDES}  )


########### next target ###############


########### install files ###############






#original Makefile.am contents follow:

#noinst_LTLIBRARIES = libui.la
#
#
#INCLUDES = -I$(top_srcdir)/kivio/kiviopart -I.. \
#	-I$(top_srcdir)/kivio/kiviopart/kiviosdk \
#	-I$(top_srcdir)/kivio/kiviopart/config \
#	-I$(top_builddir)/kivio/kiviopart/config \
#	$(KOFFICE_INCLUDES) \
#	$(KOTEXT_INCLUDES) \
#	$(all_includes)
#
#libui_la_METASOURCES = AUTO
#
#libui_la_SOURCES = aligndialog.ui kivio_birdeye_panel.cpp \
#		kivio_birdeye_panel_base.ui kivio_layer_panel_base.ui kivio_layer_panel.cpp \
#		kivio_protection_panel_base.ui kivio_protection_panel.cpp kiviooptionsdialog.cpp \
#		export_page_dialog_base.ui export_page_dialog.cpp kiviotextformatdlg.cpp kiviostencilformatdlg.cpp \
#		kivioarrowheadformatdlg.cpp kivioaligndialog.cpp kivioaddstencilsetpanel.cpp \
#		kiviostencilsetwidget.ui objectlistpalette.cpp addstencilsetdialog.cpp layerlisttooltip.cpp
#noinst_HEADERS = kivioaddstencilsetpanel.h objectlistpalette.h \
#		addstencilsetdialog.h layerlisttooltip.h
