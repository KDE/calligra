noinst_LTLIBRARIES = libkdgantt.la

libkdgantt_la_SOURCES = \
			KDGanttView.cpp			\
			KDGanttViewEventItem.cpp	\
			KDGanttViewItem.cpp		\
			KDGanttViewItemDrag.cpp		\
			KDGanttViewSubwidgets.cpp	\
			KDGanttViewSummaryItem.cpp	\
			KDGanttViewTaskItem.cpp		\
			KDGanttViewTaskLink.cpp		\
			KDGanttViewTaskLinkGroup.cpp	\
			KDGanttMinimizeSplitter.cpp		\
			KDGanttSemiSizingControl.cpp		\
			KDGanttSizingControl.cpp		\
			KDGanttXMLTools.cpp			\
			itemAttributeDialog.ui
libkdgantt_la_LDFLAGS = $(all_libraries) -no-undefined
libkdgantt_la_LIBADD = $(LIB_KDEUI)

noinst_HEADERS =	\
			KDGanttViewSubwidgets.h		\
			KDGanttMinimizeSplitter.h		\
			KDGanttSemiSizingControl.h		\
			KDGanttSizingControl.h		\
			KDGanttViewEventItem.h		\
			KDGanttView.h			\
			KDGanttViewItem.h		\
			KDGanttViewItemDrag.h		\
			KDGanttViewSummaryItem.h	\
			KDGanttViewTaskItem.h		\
			KDGanttViewTaskLinkGroup.h	\
			KDGanttViewTaskLink.h		\
			KDGanttXMLTools.h			\
			itemAttributeDialog.ui.h	\
			qlayoutengine_p.h

METASOURCES = AUTO

INCLUDES  = $(all_includes)

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kdgantt.pot
include $(top_srcdir)/doc/api/Doxyfile.am
