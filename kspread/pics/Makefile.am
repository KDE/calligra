
picsdir = $(kde_datadir)/kspread/pics
pics_DATA = abort.xpm bold.xpm center.xpm chart.xpm colin.xpm \
	colout.xpm done.xpm italic.xpm left.xpm money.xpm \
	multirow.xpm neonpen.xpm paste.xpm percent.xpm precminus.xpm \
	precplus.xpm right.xpm rowin.xpm rowout.xpm tab_firs.xpm \
	tab_first.xpm tab_last.xpm tab_left.xpm tab_righ.xpm tab_right.xpm

icon_DATA = kspread.xpm
icondir = $(kde_icondir)

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) kspread-mini.xpm $(kde_minidir)/kspread.xpm	

uninstall-local:
	-rm -f $(kde_minidir)/kspread.xpm
