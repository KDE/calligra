




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kword/templates/Normal/.source FILES  TwoColumns.kwt Letter.kwt A4.kwt Memo.kwt TwoColumnsLetter.kwt      ProfessionalLetter.odt ProfessionalA4.odt FaxA4.odt FaxLetter.odt      ColorfulA4.odt ColorfulLetter.odt PlainText.kwt )
install_files( ${DATA_INSTALL_DIR}/kword/templates/Normal FILES  .directory TwoColumns.desktop Letter.desktop A4.desktop Memo.desktop      TwoColumnsLetter.desktop ProfessionalLetter.desktop ProfessionalA4.desktop      FaxA4.desktop FaxLetter.desktop      ColorfulA4.desktop ColorfulLetter.desktop )
install_files( ${DATA_INSTALL_DIR}/kword/icons FILES  )





#original Makefile.am contents follow:

#templates_DATA = .directory TwoColumns.desktop Letter.desktop A4.desktop Memo.desktop \
#    TwoColumnsLetter.desktop ProfessionalLetter.desktop ProfessionalA4.desktop \
#    FaxA4.desktop FaxLetter.desktop \
#    ColorfulA4.desktop ColorfulLetter.desktop
#templatesdir = $(kde_datadir)/kword/templates/Normal
#
#templatesrc_DATA = TwoColumns.kwt Letter.kwt A4.kwt Memo.kwt TwoColumnsLetter.kwt \
#    ProfessionalLetter.odt ProfessionalA4.odt FaxA4.odt FaxLetter.odt \
#    ColorfulA4.odt ColorfulLetter.odt PlainText.kwt
#templatesrcdir = $(kde_datadir)/kword/templates/Normal/.source
#
#kworddir = $(kde_datadir)/kword/icons
#kword_ICON = AUTO
