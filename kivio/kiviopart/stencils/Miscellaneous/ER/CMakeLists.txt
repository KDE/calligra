kde4_header()




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kivio/stencils/Miscellaneous/ER FILES  desc icon.png entity.sml weakentity.sml attribute.sml multivaluedattribute.sml  	derivedattribute.sml relationship.sml weakrelationship.sml )

kde4_footer()



#original Makefile.am contents follow:

#erdir = $(kde_datadir)/kivio/stencils/Miscellaneous/ER
#
#er_DATA = desc icon.png entity.sml weakentity.sml attribute.sml multivaluedattribute.sml \
#	derivedattribute.sml relationship.sml weakrelationship.sml
