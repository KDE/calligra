SUBDIRS = kiviopart plugins templates

package-messages:
	$(MAKE) -f admin/Makefile.common package-messages
	$(MAKE) -C po merge


messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui` >> rc.cpp
	$(EXTRACTRC) `find . -name \*.rc` >> rc.cpp
	$(EXTRACTRC) --tag=name `find . -name \*.shape` >> rc.cpp
	$(XGETTEXT) `find . -name \*.cpp -not -name \*.moc.*` kiviopart/kivio_aboutdata.h -o $(podir)/kivio.pot
