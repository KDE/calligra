

include_directories( ${CMAKE_SOURCE_DIR}/kivio/kiviopart ${CMAKE_SOURCE_DIR}/kivio/kiviopart/kiviosdk ${KDE4_INCLUDES}  )


########### next target ###############


########### install files ###############

install_files( ${KCFG_INSTALL_DIR} FILES  kivio.kcfg )





#original Makefile.am contents follow:

#noinst_LTLIBRARIES = libkivioconfig.la
#INCLUDES = -I$(top_srcdir)/kivio/kiviopart -I.. \
#	-I$(top_srcdir)/kivio/kiviopart/kiviosdk \
#	$(KOFFICE_INCLUDES) \
#	$(KOTEXT_INCLUDES) \
#	$(all_includes)
#
#libkivioconfig_la_METASOURCES = AUTO
#
#libkivioconfig_la_SOURCES = kivio_settings.kcfgc kivio_config.cpp
#noinst_HEADERS = kivio_config.h kivio_settings.h
#
#kde_kcfg_DATA = kivio.kcfg
