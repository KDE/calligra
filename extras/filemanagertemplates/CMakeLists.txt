########### install files ###############

install( FILES
   Presentation.desktop
   TextDocument.desktop
   SpreadSheet.desktop
   Illustration.desktop
DESTINATION ${TEMPLATES_INSTALL_DIR})

install( FILES
   Presentation.odp
   TextDocument.odt
   SpreadSheet.ods
   Illustration.odg
DESTINATION ${TEMPLATES_INSTALL_DIR}/.source)

