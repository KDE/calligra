




########### install files ###############

install( FILES  GradientBlueRed.kpt  	kde.kpt kde2.kpt kde3.kpt classroom.kpt savannah.kpt  	SnowyMountains.kpt TotallyNewProduct.kpt  	BlueBreezeDouble.kpt BlueBreezePicture.kpt BlueBreezeSingle.kpt  	CopperPlain.kpt  DESTINATION ${DATA_INSTALL_DIR}/kpresenter/templates/Screenpresentations/.source)
install( FILES  .directory GradientBlueRed.desktop  	kde.desktop kde2.desktop kde3.desktop classroom.desktop savannah.desktop  	SnowyMountains.desktop TotallyNewProduct.desktop  	BlueBreezeDouble.desktop BlueBreezePicture.desktop BlueBreezeSingle.desktop CopperPlain.desktop  DESTINATION ${DATA_INSTALL_DIR}/kpresenter/templates/Screenpresentations)
install( FILES   DESTINATION ${DATA_INSTALL_DIR}/kpresenter/icons)





#original Makefile.am contents follow:

#
#template_DATA = .directory GradientBlueRed.desktop \
#	kde.desktop kde2.desktop kde3.desktop classroom.desktop savannah.desktop \
#	SnowyMountains.desktop TotallyNewProduct.desktop \
#	BlueBreezeDouble.desktop BlueBreezePicture.desktop BlueBreezeSingle.desktop CopperPlain.desktop
#
#templatedir = $(kde_datadir)/kpresenter/templates/Screenpresentations
#
#templatesrc_DATA = GradientBlueRed.kpt \
#	kde.kpt kde2.kpt kde3.kpt classroom.kpt savannah.kpt \
#	SnowyMountains.kpt TotallyNewProduct.kpt \
#	BlueBreezeDouble.kpt BlueBreezePicture.kpt BlueBreezeSingle.kpt \
#	CopperPlain.kpt
#
#templatesrcdir = $(kde_datadir)/kpresenter/templates/Screenpresentations/.source
#
#kpricondir = $(kde_datadir)/kpresenter/icons
#kpricon_ICON = AUTO
