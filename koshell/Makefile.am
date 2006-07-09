
INCLUDES  = $(KOFFICE_INCLUDES) $(all_includes)

## The kdeinit loadable module and executable
lib_LTLIBRARIES = 
kdeinit_LTLIBRARIES = koshell.la
bin_PROGRAMS =
koshell_la_SOURCES = koshell_main.cc koshell_shell.cc iconsidepane.cpp koshellsettings.kcfgc
koshell_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
koshell_la_LIBADD = $(LIB_KOFFICEUI)
kde_kcfg_DATA = koshell.kcfg

METASOURCES = AUTO

KDE_ICON=AUTO

rcdir = $(kde_datadir)/koshell
rc_DATA = koshellui.rc

applnk_DATA = koshell.desktop
applnkdir = $(kde_appsdir)/Office

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc *.cpp -o $(podir)/koshell.pot

include $(top_srcdir)/admin/Doxyfile.am
