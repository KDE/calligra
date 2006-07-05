




########### install files ###############

install( FILES  sample1.ktf sample2.ktf sample3.ktf sample1.kdf sample2.kdf sample3.kdf  DESTINATION ${DATA_INSTALL_DIR}/kugar/templates)





#original Makefile.am contents follow:

#templatedir = $(kde_datadir)/kugar/templates
#template_DATA = sample1.ktf sample2.ktf sample3.ktf sample1.kdf sample2.kdf sample3.kdf
#
#EXTRA_DIST = $(template_DATA)
