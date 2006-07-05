




########### install files ###############

install( FILES  x-sqlite2.desktop   x-sqlite3.desktop  DESTINATION ${MIME_INSTALL_DIR}/application)
install( FILES  sqlite.magic msaccess.magic  DESTINATION ${CONFIG_INSTALL_DIR}/magic)





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
