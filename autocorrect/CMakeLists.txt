




########### install files ###############

install( FILES  autocorrect.xml en_US.xml  DESTINATION ${DATA_INSTALL_DIR}/koffice/autocorrect)





#original Makefile.am contents follow:

## (Is autocorrect.xml still used?)
#
## WARNING: Do not add any other language here - use kde-i18n/*/data/koffice/autocorrect for that.
#scripts_DATA = autocorrect.xml en_US.xml
#scriptsdir = $(kde_datadir)/koffice/autocorrect
#
