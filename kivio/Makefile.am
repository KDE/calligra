SUBDIRS = kiviopart plugins templates

# Note: kivio-sml-i18n.cpp is only needed for I18N, not for building

package-messages:
	$(MAKE) -f admin/Makefile.common package-messages
	$(MAKE) -C po merge


messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui` >> rc.cpp
	$(EXTRACTRC) `find . -name \*.rc` >> rc.cpp
	$(EXTRACTRC) --tag=name --context=Stencils `find . -name \*.shape` >> rc.cpp
	$(XGETTEXT) `find . -name \*.cpp -not -name \*.moc.*` kiviopart/kivio_aboutdata.h kivio-sml-i18n.cpp kivio-desc-i18n.cpp -o $(podir)/kivio.pot
