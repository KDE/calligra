#1########################################################!@#$
####   Makefile.am generated automatically by KDEStudio   ####
#### WARNING! All changes made in this part will be lost! ####
##############################################################
SUBDIRS = kiviopart plugins 
EXTRA_DIST = kivio.studio res.cpp
##############################################################
####                  END KDEStudio part                  ####
#2########################################################!@#$

package-messages:
	$(MAKE) -f admin/Makefile.common package-messages
	$(MAKE) -C po merge


messages: rc.cpp
	$(XGETTEXT) `find . -name \*.cpp -not -name \*.moc.*` kiviopart/kivio_aboutdata.h -o $(podir)/kivio.pot
