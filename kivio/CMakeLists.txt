
project(kivio)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-kivio.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kivio.h)

#add_subdirectory( kiviopart )
#add_subdirectory( plugins )
#add_subdirectory( templates )

add_subdirectory(src)


########### install files ###############








#original Makefile.am contents follow:

#SUBDIRS = kiviopart plugins templates
#
## Note: kivio-sml-i18n.cpp and kivio-extract-18n-desc.pl
##  are only needed for I18N, neither for building nor for run-time
#
#package-messages:
#	$(MAKE) -f admin/Makefile.common package-messages
#	$(MAKE) -C po merge
#
#
#messages: rc.cpp
#	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` >> rc.cpp
#	perl -n kivio-extract-i18n-desc.pl `find . -name desc -o -name \*.sml` >> rc.cpp
#	$(EXTRACTRC) --tag-group=none --tag=name --context=Stencils `find . -name \*.shape` >> rc.cpp
#	$(XGETTEXT) `find . -name \*.cpp -not -name \*.moc.\*` kiviopart/kivio_aboutdata.h -o $(podir)/kivio.pot
#
#include $(top_srcdir)/admin/Doxyfile.am
