




########### install files ###############

install_files( ${MIME_INSTALL_DIR}/application FILES  x-sqlite2.desktop   x-sqlite3.desktop )
install_files( ${CONFIG_INSTALL_DIR}/magic FILES  sqlite.magic msaccess.magic )





#original Makefile.am contents follow:

#application_DATA = \
# x-sqlite2.desktop \
# x-sqlite3.desktop
#
#applicationdir = $(kde_mimedir)/application
#
#magic_DATA = sqlite.magic msaccess.magic
#magicdir = $(kde_confdir)/magic
#
#EXTRA_DIST = $(application_DATA)
