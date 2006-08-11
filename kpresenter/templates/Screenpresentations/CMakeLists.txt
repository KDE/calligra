
install( FILES  GradientBlueRed.kpt  	kde.kpt kde2.kpt kde3.kpt classroom.kpt savannah.kpt  	SnowyMountains.kpt TotallyNewProduct.kpt  	BlueBreezeDouble.kpt BlueBreezePicture.kpt BlueBreezeSingle.kpt  	CopperPlain.kpt  DESTINATION ${DATA_INSTALL_DIR}/kpresenter/templates/Screenpresentations/.source)

install( FILES  .directory GradientBlueRed.desktop  	kde.desktop kde2.desktop kde3.desktop classroom.desktop savannah.desktop  	SnowyMountains.desktop TotallyNewProduct.desktop  	BlueBreezeDouble.desktop BlueBreezePicture.desktop BlueBreezeSingle.desktop CopperPlain.desktop  DESTINATION ${DATA_INSTALL_DIR}/kpresenter/templates/Screenpresentations)

kde4_install_icons( ${DATA_INSTALL_DIR}/kpresenter/icons hicolor )

