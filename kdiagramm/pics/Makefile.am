
picsdir = $(kde_datadir)/kdiagramm/pics
pics_DATA = bars.xpm areas.xpm lines.xpm cakes.xpm

kde_icon_DATA = kdiagramm.xpm

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/kdiagramm-mini.xpm $(kde_minidir)/kdiagramm.xpm

uninstall-local:
	-rm -f $(kde_minidir)/kdiagramm.xpm
