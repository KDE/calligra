




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kpresenter/templates/A4/.source FILES  OneColumnLandscape.kpt   OneColumnPortrait.kpt   TitleLandscape.kpt   TitlePortrait.kpt   TwoColumnLandscape.kpt   TwoColumnPortrait.kpt   emptyLandscape.kpt   emptyPortrait.kpt )





#original Makefile.am contents follow:

#
#install-data-local:
#	$(mkinstalldirs) $(DESTDIR)$(kde_datadir)/kpresenter/templates/A4
#	for file in OneColumnLandscape.desktop TitleLandscape.desktop \
#	   TwoColumnLandscape.desktop emptyLandscape.desktop OneColumnPortrait.desktop \
#	   TitlePortrait.desktop TwoColumnPortrait.desktop emptyPortrait.desktop; do \
#	  $(INSTALL_DATA) $(srcdir)/../common_desktop/$$file $(DESTDIR)$(kde_datadir)/kpresenter/templates/A4/$$file ;\
#	done
#	$(INSTALL_DATA) $(srcdir)/.directory $(DESTDIR)$(kde_datadir)/kpresenter/templates/A4
#
#templatesrc_DATA = \
# OneColumnLandscape.kpt \
# OneColumnPortrait.kpt \
# TitleLandscape.kpt \
# TitlePortrait.kpt \
# TwoColumnLandscape.kpt \
# TwoColumnPortrait.kpt \
# emptyLandscape.kpt \
# emptyPortrait.kpt
#templatesrcdir = $(kde_datadir)/kpresenter/templates/A4/.source
