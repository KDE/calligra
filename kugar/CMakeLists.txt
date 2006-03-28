kde4_header()

add_subdirectory( lib )
add_subdirectory( part )
add_subdirectory( samples )
add_subdirectory( kudesigner_lib )
add_subdirectory( kudesigner )




########### install files ###############


kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

kde4_footer()



#original Makefile.am contents follow:

##SUBDIRS = lib part shell samples kudesigner
#SUBDIRS = lib part samples kudesigner_lib kudesigner
#
#KDE_ICON = kugar kugardata
#
#messages: 
#	$(EXTRACTRC) lib/*.ui part/*.ui kudesigner/*.ui */*.rc > rc.cpp
#	$(XGETTEXT) -ktranslate lib/*.cpp part/*.cpp kudesigner/*.cpp kudesigner_lib/*.cpp kudesigner_lib/*.h rc.cpp -o $(podir)/kugar.pot
