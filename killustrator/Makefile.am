# This file you have to edit. Change the name here
SUBDIRS = share filter xmlutils standalone koffice pics doc

# not a GNU package. You can remove this line, if 
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign

# only Tom may know, why make dist forgets configure
EXTRA_DIST = README kil.dtd killustrator.lsm configure

messages:
	$(XGETTEXT) -C -ktranslate -ki18n -x$(includedir)/kde.pot */*.cc && mv messages.po ../po/killustrator.pot

dist-hook: $(top_srcdir)/automoc
	cp $(top_srcdir)/automoc $(top_distdir)/automoc
	cd $(top_distdir) && perl automoc
