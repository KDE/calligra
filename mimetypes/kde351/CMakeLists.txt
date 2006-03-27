kde4_header()




########### install files ###############

install_files( ${MIME_INSTALL_DIR}/image FILES  x-raw.desktop )

kde4_footer()



#original Makefile.am contents follow:

#image_DATA = x-raw.desktop
#
#imagedir = $(kde_mimedir)/image
#
#EXTRA_DIST = $(image_DATA)
