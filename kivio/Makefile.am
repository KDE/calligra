#1########################################################!@#$
####   Makefile.am generated automatically by KDEStudio   ####
#### WARNING! All changes made in this part will be lost! ####
##############################################################
SUBDIRS = kiviopart plugins 
EXTRA_DIST = admin/* Makefile.cvs kivio.studio \
		configure.in.in\
		res.cpp
##############################################################
####                  END KDEStudio part                  ####
#2########################################################!@#$

package-messages:
	$(MAKE) -f admin/Makefile.common package-messages
	$(MAKE) -C po merge


messages:
	$(XGETTEXT) `find . -name \*.cpp -not -name \*.moc.*` -o $(podir)/kivio.pot
