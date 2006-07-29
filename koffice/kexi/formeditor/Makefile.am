include $(top_srcdir)/kexi/Makefile.global

INCLUDES = -I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/widget/utils \
  -I$(top_srcdir)/kexi/widget/tableview \
  -I$(top_srcdir)/kexi/core \
  -I$(top_srcdir)/kexi \
  -I$(top_srcdir)/lib/kofficecore \
  -I$(top_srcdir)/lib -I$(top_srcdir)/lib/koproperty $(all_includes)

lib_LTLIBRARIES =  libkformdesigner.la
libkformdesigner_la_LDFLAGS = -no-undefined $(all_libraries) $(VER_INFO)
libkformdesigner_la_SOURCES = container.cpp resizehandle.cpp widgetfactory.cpp \
   widgetlibrary.cpp libactionwidget.cpp form.cpp \
  objecttree.cpp formIO.cpp formmanager.cpp objecttreeview.cpp spring.cpp \
  commands.cpp events.cpp connectiondialog.cpp richtextdialog.cpp \
  tabstopdialog.cpp editlistviewdialog.cpp utils.cpp widgetpropertyset.cpp \
  kfdpixmapedit.cpp widgetwithsubpropertiesinterface.cpp

METASOURCES = AUTO
libkformdesigner_la_LIBADD = \
  $(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
  $(top_builddir)/kexi/widget/utils/libkexiguiutils.la \
  $(top_builddir)/kexi/widget/tableview/libkexidatatable.la \
  $(top_builddir)/lib/koproperty/libkoproperty.la \
  $(LIB_KPARTS)

servicetypesdir = $(kde_servicetypesdir)
servicetypes_DATA=widgetfactory.desktop

SUBDIRS = . factories #test #scripting #kdevelop_plugin

messages: rc.cpp
	$(EXTRACTRC) `find . -name "*.rc" -o -name "*.ui"` >> rc.cpp
	$(XGETTEXT) `find . -name "*.cpp" -o -name "*.h"` -o $(podir)/kformdesigner.pot
