SUBDIRS = kiviopart plugins templates

# Note: kivio-sml-i18n.cpp and kivio-extract-18n-desc.pl
#  are only needed for I18N, neither for building nor for run-time

package-messages:
	$(MAKE) -f admin/Makefile.common package-messages
	$(MAKE) -C po merge


messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui` >> rc.cpp
	$(EXTRACTRC) `find . -name \*.rc` >> rc.cpp
	perl -n kivio-extract-i18n-desc.pl `find . -name desc` >> rc.cpp
	$(EXTRACTRC) --tag=name --context=Stencils `find . -name \*.shape` >> rc.cpp
	$(XGETTEXT) `find . -name \*.cpp -not -name \*.moc.*` kiviopart/kivio_aboutdata.h kivio-sml-i18n.cpp kivio-desc-i18n.cpp -o $(podir)/kivio.pot
