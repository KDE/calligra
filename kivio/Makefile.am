#1########################################################!@#$
####   Makefile.am generated automatically by KDEStudio   ####
#### WARNING! All changes made in this part will be lost! ####
##############################################################
SUBDIRS = po kiviopart plugins 
EXTRA_DIST = admin/* Makefile.cvs kivio.studio \
		configure.in.in\
		res.cpp
##############################################################
####                  END KDEStudio part                  ####
#2########################################################!@#$
AUTOMAKE_OPTIONS = foreign 1.4

$(top_srcdir)/acinclude.m4: $(top_srcdir)/admin/acinclude.m4.in $(top_srcdir)/admin/libtool.m4.in
	@cd $(top_srcdir) && cat admin/acinclude.m4.in admin/libtool.m4.in > acinclude.m4

dist-hook:
	cd $(top_distdir) && perl $(top_srcdir)/admin/am_edit -padmin

$(top_srcdir)/configure.in: $(top_srcdir)/configure.files $(shell test -f $(top_srcdir)/configure.files && cat $(top_srcdir)/configure.files) $(top_srcdir)/subdirs
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common configure.in
$(top_srcdir)/subdirs:
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common subdirs
$(top_srcdir)/configure.files:
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common configure.files

MAINTAINERCLEANFILES = subdirs configure.in acinclude.m4 SUBDIRS

package-messages:
	$(MAKE) -f admin/Makefile.common package-messages
	$(MAKE) -C po merge


messages:
	$(XGETTEXT) `find . -name \*.cpp -not -name \*.moc.*` -o $(podir)/kivio.pot
