icon_DATA = kspread.xpm
icondir = $(kde_icondir)

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/kspread-mini.xpm $(kde_minidir)/kspread.xpm	

uninstall-local:
	-rm -f $(kde_minidir)/kspread.xpm

