kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/stencils/UML/ClassDiagrams FILES  ClassConceptual.sml  	icon.xpm  	Note.sml  	ClassConceptual.xpm  	Interface.sml  	Note.xpm  	Class.sml  	Interface.xpm  	TemplateClass.sml  	Class.xpm  	TemplateClass.xpm  	desc )

kde4_footer()



#original Makefile.am contents follow:

#classdir = $(kde_datadir)/kivio/stencils/UML/ClassDiagrams
#
#class_DATA = \
#	ClassConceptual.sml \
#	icon.xpm \
#	Note.sml \
#	ClassConceptual.xpm \
#	Interface.sml \
#	Note.xpm \
#	Class.sml \
#	Interface.xpm \
#	TemplateClass.sml \
#	Class.xpm \
#	TemplateClass.xpm \
#	desc
#
#EXTRA_DIST = \
#	ClassConceptual.sml \
#	icon.xpm \
#	Note.sml \
#	ClassConceptual.xpm \
#	Interface.sml \
#	Note.xpm \
#	Class.sml \
#	Interface.xpm \
#	TemplateClass.sml \
#	Class.xpm \
#	TemplateClass.xpm \
#	desc
