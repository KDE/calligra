########### install files ###############

install( FILES
   Presentation.desktop
   TextDocument.desktop
   SpreadSheet.desktop
   Illustration.desktop
DESTINATION ${KDE_INSTALL_TEMPLATEDIR})

install( FILES
   Presentation.odp
   TextDocument.odt
   SpreadSheet.ods
   Illustration.odg
DESTINATION ${KDE_INSTALL_TEMPLATEDIR}/.source)

