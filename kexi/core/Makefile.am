lib_LTLIBRARIES = libkexicore.la
libkexicore_la_SOURCES = kexiproject.cpp keximainwindow.cpp kexidialogbase.cpp \
	kexistartupdlgui.ui kexistartupdlg.cpp kexiprojectconnectiondata.cpp \
	kexipartmanager.cpp kexipartinfo.cpp kexipartitem.cpp kexipart.cpp \
	kexibrowser.cpp kexibrowseritem.cpp
libkexicore_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) -Wnounresolved -no-undefined
libkexicore_la_LIBADD = -lkmdi ../kexidb/libkexidb.la \
	../3rdparty/uuid/libkexiuuid.la

INCLUDES= $(KOFFICE_INCLUDES) -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/core/projectWizard \
-I$(top_srcdir)/kexi/3rdparty/uuid $(all_includes) 

METASOURCES = AUTO

include $(top_srcdir)/kexi/Makefile.global

