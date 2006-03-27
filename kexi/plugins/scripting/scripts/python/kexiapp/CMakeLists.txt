kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kexi/kross/python/kexiapp FILES  __init__.py )

kde4_footer()



#original Makefile.am contents follow:

#kexiapppythondir = $(kde_datadir)/kexi/kross/python/kexiapp
#kexiapppython_SCRIPTS = __init__.py
