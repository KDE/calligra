




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kpresenter/autoforms/Arrows/.icon FILES  ArrowDown.png ArrowLeft.png  	ArrowLeftDown.png ArrowLeftUp.png  	ArrowRight.png ArrowRightDown.png  	ArrowRightUp.png ArrowUp.png )
install_files( ${DATA_INSTALL_DIR}/kpresenter/autoforms/Arrows FILES  .directory ArrowDown.desktop ArrowLeft.desktop  	ArrowLeftDown.desktop  ArrowLeftUp.desktop  	ArrowRight.desktop ArrowRightDown.desktop  	ArrowRightUp.desktop ArrowUp.desktop )
install_files( ${DATA_INSTALL_DIR}/kpresenter/autoforms/Arrows/.source FILES  ArrowDown.atf ArrowLeft.atf  	ArrowLeftDown.atf ArrowLeftUp.atf  	ArrowRight.atf ArrowRightDown.atf  	ArrowRightUp.atf ArrowUp.atf )





#original Makefile.am contents follow:

#
#autoforms_DATA = .directory ArrowDown.desktop ArrowLeft.desktop \
#	ArrowLeftDown.desktop  ArrowLeftUp.desktop \
#	ArrowRight.desktop ArrowRightDown.desktop \
#	ArrowRightUp.desktop ArrowUp.desktop
#autoformsdir = $(kde_datadir)/kpresenter/autoforms/Arrows
#
#autoformsrc_DATA = ArrowDown.atf ArrowLeft.atf \
#	ArrowLeftDown.atf ArrowLeftUp.atf \
#	ArrowRight.atf ArrowRightDown.atf \
#	ArrowRightUp.atf ArrowUp.atf
#autoformsrcdir = $(kde_datadir)/kpresenter/autoforms/Arrows/.source
#
#autoformsicons_DATA = ArrowDown.png ArrowLeft.png \
#	ArrowLeftDown.png ArrowLeftUp.png \
#	ArrowRight.png ArrowRightDown.png \
#	ArrowRightUp.png ArrowUp.png
#autoformsiconsdir = $(kde_datadir)/kpresenter/autoforms/Arrows/.icon
