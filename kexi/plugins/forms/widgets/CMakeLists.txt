

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/plugins/forms ${CMAKE_SOURCE_DIR}/kexi/core ${KDE4_INCLUDES}  )


########### next target ###############


########### install files ###############






#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#noinst_LTLIBRARIES = libkexiformutilswidgets.la
#
#libkexiformutilswidgets_la_SOURCES = \
#  kexidbautofield.cpp \
#  kexidbform.cpp \
#  kexidbsubform.cpp \
#  kexidblabel.cpp \
#  kexidbimagebox.cpp \
#  kexipushbutton.cpp \
#  kexiframe.cpp \
#  kexidbdoublespinbox.cpp \
#  kexidbintspinbox.cpp \
#  kexidbdatetimeedit.cpp \
#  kexidbtimeedit.cpp \
#  kexidbdateedit.cpp \
#  kexidblineedit.cpp \
#  kexidbcheckbox.cpp \
#  kexidbtextedit.cpp 
#
#libkexiformutilswidgets_la_LDFLAGS = $(all_libraries) -Wnounresolved
#libkexiformutilswidgets_la_LIBADD = 
#
#libkexiformutilswidgets_la_METASOURCES = AUTO
#
#SUBDIRS = .
#
## set the include path for X, qt and KDE
#INCLUDES= -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/plugins/forms -I$(top_srcdir)/kexi/core $(all_includes)
#
