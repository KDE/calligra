
pics_DATA =  kautoformedit.xpm 
picsdir = $(kde_datadir)/kpresenter/pics

icon_DATA = kpresenter.xpm
icondir = $(kde_icondir)

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) kpresenter-mini.xpm $(kde_minidir)/kpresenter.xpm	

uninstall-local:
	-rm -f $(kde_minidir)/kpresenter.xpm

