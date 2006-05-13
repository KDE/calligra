




########### install files ###############

install_files( ${TEMPLATES_INSTALL_DIR} FILES  Presentation.desktop TextDocument.desktop SpreadSheet.desktop Illustration.desktop )
install_files( ${TEMPLATES_INSTALL_DIR}/.source FILES  Presentation.kpt TextDocument.kwt SpreadSheet.kst Illustration.karbon )





#original Makefile.am contents follow:

#
#templ_DATA = Presentation.desktop TextDocument.desktop SpreadSheet.desktop Illustration.desktop
#
#templdir = $(kde_templatesdir)
#
#sources_DATA = Presentation.kpt TextDocument.kwt SpreadSheet.kst Illustration.karbon
#sourcesdir = $(kde_templatesdir)/.source
