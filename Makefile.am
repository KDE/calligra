
SUBDIRS = lib kspread kpresenter kword kformula kdiagramm killustrator kimage kohtml tools plugins trader koshell pics

AUTOMAKE_OPTIONS = foreign # 1.4

acinclude.m4: admin/acinclude.m4.in admin/libtool.m4.in
	@cat $(top_srcdir)/admin/acinclude.m4.in $(top_srcdir)/admin/libtool.m4.in   > $(top_srcdir)/acinclude.m4

messages:
	rm -rf po.backup
	mkdir po.backup
	for i in `ls -1 po/*.pot | sed -e "s#po/##"`; do \
	  grep -v "\"POT-Creation" po/$$i > po.backup/$$i ; \
	  cp po/$$i po.backup/backup_$$i ;  \
	done
	files=`ls -1`; \
	dirs=`for i in $$files; do if test -d $$i && test -r $$i/Makefile.am; then echo $$i; fi; done`; \
	for subdir in $$dirs; do \
	  echo "Making messages in $$subdir"; \
	  (cd $$subdir && $(MAKE) -f Makefile.am messages) \
	done
	for  i in `ls -1 po/*.pot | sed -e "s#po/##"`; do \
	  grep -v "POT-Creation-Date:" po/$$i > temp.pot ;\
	  if diff -q temp.pot po.backup/$$i; then \
	     echo "I'm restoring $$i" ;\
	     mv po.backup/backup_$$i po/$$i; \
	   else \
	     echo "will leave $$i"; \
	fi ;\
	done
	rm -f temp.pot
	rm -rf po.backup
	(cd po && $(MAKE) -f Makefile.am merge)

dist-hook:
	cd $(top_distdir) && perl $(top_srcdir)/admin/am_edit -padmin

