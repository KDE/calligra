




########### install files ###############

install( FILES  Presentation.desktop TextDocument.desktop SpreadSheet.desktop Illustration.desktop  DESTINATION ${TEMPLATES_INSTALL_DIR})
install( FILES  Presentation.kpt TextDocument.kwt SpreadSheet.kst Illustration.karbon  DESTINATION ${TEMPLATES_INSTALL_DIR}/.source)





#original Makefile.am contents follow:

#
#templ_DATA = Presentation.desktop TextDocument.desktop SpreadSheet.desktop Illustration.desktop
#
#templdir = $(kde_templatesdir)
#
#sources_DATA = Presentation.kpt TextDocument.kwt SpreadSheet.kst Illustration.karbon
#sourcesdir = $(kde_templatesdir)/.source
