
pics_DATA =  kautoformedit.xpm 
picsdir = $(kde_datadir)/kpresenter/pics

kde_icon_DATA = kpresenter.xpm

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/kpresenter-mini.xpm $(kde_minidir)/kpresenter.xpm

uninstall-local:
	-rm -f $(kde_minidir)/kpresenter.xpm

