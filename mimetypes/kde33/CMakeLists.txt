kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${MIME_INSTALL_DIR}/application FILES  vnd.oasis.opendocument.graphics-template.desktop   vnd.oasis.opendocument.graphics.desktop   vnd.oasis.opendocument.formula.desktop   vnd.oasis.opendocument.presentation-template.desktop   vnd.oasis.opendocument.presentation.desktop   vnd.oasis.opendocument.spreadsheet-template.desktop   vnd.oasis.opendocument.spreadsheet.desktop   vnd.oasis.opendocument.text-template.desktop   vnd.oasis.opendocument.text.desktop   vnd.oasis.opendocument.image.desktop   vnd.oasis.opendocument.chart.desktop )

kde4_footer()



#original Makefile.am contents follow:

#application_DATA = \
# vnd.oasis.opendocument.graphics-template.desktop \
# vnd.oasis.opendocument.graphics.desktop \
# vnd.oasis.opendocument.formula.desktop \
# vnd.oasis.opendocument.presentation-template.desktop \
# vnd.oasis.opendocument.presentation.desktop \
# vnd.oasis.opendocument.spreadsheet-template.desktop \
# vnd.oasis.opendocument.spreadsheet.desktop \
# vnd.oasis.opendocument.text-template.desktop \
# vnd.oasis.opendocument.text.desktop \
# vnd.oasis.opendocument.image.desktop \
# vnd.oasis.opendocument.chart.desktop
#
#applicationdir = $(kde_mimedir)/application
#
#EXTRA_DIST = $(application_DATA)
