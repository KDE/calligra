
data_DATA = network_settings.xpm
datadir = $(kde_toolbardir)

kde_icon_DATA = ktables.xpm

install-data-local: 
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-ktables.xpm $(kde_minidir)/ktables.xpm

uninstall-local:
	-rm -f $(kde_minidir)/ktables.xpm

