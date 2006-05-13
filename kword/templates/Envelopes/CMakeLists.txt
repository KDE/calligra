




########### install files ###############

install_files(  ${DATA_INSTALL_DIR}/kword/templates/Envelopes FILES  EnvelopeC6.kwt EnvelopeDL.kwt )
install_files(  ${DATA_INSTALL_DIR}/kword/templates/Envelopes FILES  .directory EnvelopeC6.desktop EnvelopeDL.desktop )
install_files( ${DATA_INSTALL_DIR}/kword/icons FILES  )





#original Makefile.am contents follow:

#templatesubdir = Envelopes
#
#templates_DATA = .directory EnvelopeC6.desktop EnvelopeDL.desktop
#templatesdir = $(kde_datadir)/kword/templates/$(templatesubdir)
#
#templatesrc_DATA = EnvelopeC6.kwt EnvelopeDL.kwt
#templatesrcdir = $(kde_datadir)/kword/templates/$(templatesubdir)/.source
#
#kworddir = $(kde_datadir)/kword/icons
#kword_ICON = AUTO
#
