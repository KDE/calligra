
INCLUDES  = $(all_includes) $(PYTHONINC) -I$(idldir) $(MICO_INCLUDES) -I../lib/koml -I../lib/kofficecore -I../lib/kofficeui
LDFLAGS   = $(all_libraries) $(PYTHONLIB)
LDADD = -lkofficeui -lkofficecore -lkoml -lkfile -lkfm -lop -lkom -lqt -lkdeui -lkdecore $(LIBMICO) $(LIBPYTHON) -lXext -lX11 

####### Files

bin_PROGRAMS = kspread

kspread_SOURCES = kmetaedit.cc kspread_emacs.cc kspread_main.cc \
	kspread_shell.cc kspread_doc.cc kspread_dlg_paper.cc \
	kspread_dlg_paper_headings.cc kspread_dlg_paper_border.cc \
	kspread.cc kspread_undo.cc kspread_view.cc kspread_tabbar.cc \
	kspread_python.cc kspread_dlg_print.cc kspread_autofill.cc \
	kspread_layout.cc kspread_map.cc kspread_dlg_layout.cc \
	kspread_dlg_scripts.cc kspread_cell.cc kspread_table.cc

noinst_HEADERS = kmetaedit.h kspread_emacs.h kspread_main.cc \
	kspread_shell.h kspread_doc.h kspread_dlg_paper.h \
	kspread_dlg_paper_headings.h kspread_dlg_paper_border.h \
	kspread.h kspread_undo.h kspread_view.h kspread_tabbar.h \
	kspread_python.h kspread_dlg_print.h kspread_autofill.h \
	kspread_layout.h kspread_map.h kspread_dlg_layout.h \
	kspread_dlg_scripts.h kspread_table.h kspread_cell.h

SRCMETA = kspread_main.moc kspread_shell.moc kspread_doc.moc \
	kspread_dlg_paper.moc kspread_dlg_paper_headings.moc \
	kspread_dlg_paper_border.moc kspread_view.moc kspread_tabbar.moc \
	kspread_dlg_print.moc kspread_table.moc kspread_dlg_layout.moc \
	kspread_dlg_scripts.moc kspread_part.moc

idl_DATA = kspread.idl
idl_HEADERS = kspread.h

applnk_DATA = kspread_part.kdelnk
applnkdir = $(kde_datadir)/koffice/partlnk

SUBDIRS = pics scripts

####### Implicit rules

SUFFIXES = .h .moc

.h.moc: 
	$(MOC) $< -o $@

####### Build rules

idl: kspread.idl
	idl --query-server-for-narrow -I$(idldir) kspread.idl


kspread_dlg_layout.cc: kspread_dlg_layout.moc
kspread_dlg_paper.cc: kspread_dlg_paper.moc
kspread_dlg_paper_border.cc: kspread_dlg_paper_border.moc
kspread_dlg_paper_headings.cc: kspread_dlg_paper_headings.moc
kspread_dlg_print.cc: kspread_dlg_print.moc
kspread_dlg_scripts.cc: kspread_dlg_scripts.moc
kspread_doc.cc: kspread_doc.moc
kspread_main.cc: kspread_main.moc
kspread_part.cc: kspread_part.moc
kspread_shell.cc: kspread_shell.moc
kspread_tabbar.cc: kspread_tabbar.moc
kspread_table.cc: kspread_table.moc
kspread_view.cc: kspread_view.moc     
