




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kpresenter/templates/Screen/.source FILES  OneColumnLandscape.kpt   TitleLandscape.kpt   TwoColumnLandscape.kpt   emptyLandscape.kpt  )





#original Makefile.am contents follow:

#
#install-data-local:
#	$(mkinstalldirs) $(DESTDIR)$(kde_datadir)/kpresenter/templates/Screen
#	$(mkinstalldirs) $(DESTDIR)$(kde_datadir)/kpresenter/templates/Screen/.source
#	for file in OneColumnLandscape.desktop TitleLandscape.desktop \
#	   TwoColumnLandscape.desktop emptyLandscape.desktop ; do \
#	  $(INSTALL_DATA) $(srcdir)/../common_desktop/$$file $(DESTDIR)$(kde_datadir)/kpresenter/templates/Screen/$$file ;\
#	done
#	$(INSTALL_DATA) $(srcdir)/.directory $(DESTDIR)$(kde_datadir)/kpresenter/templates/Screen
#
#templatesrc_DATA = \
# OneColumnLandscape.kpt \
# TitleLandscape.kpt \
# TwoColumnLandscape.kpt \
# emptyLandscape.kpt 
#templatesrcdir = $(kde_datadir)/kpresenter/templates/Screen/.source
