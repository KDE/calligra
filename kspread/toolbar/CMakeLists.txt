kde4_header()

add_subdirectory( crystalsvg )




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kspread/icons FILES  )

kde4_footer()



#original Makefile.am contents follow:

#kspreadicondir = $(kde_datadir)/kspread/icons
#kspreadicon_ICON = AUTO
#SUBDIRS = crystalsvg
