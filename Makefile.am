
SUBDIRS = lib kspread kpresenter kword po 

acinclude.m4: admin/acinclude.m4.in admin/libtool.m4.in
	cat admin/acinclude.m4.in admin/libtool.m4.in   > acinclude.m4 

messages:
	rm -rf po.backup
	mkdir po.backup
	for i in `ls -1 po/*.pot | sed -e "s#po/##"`; do \
	  grep -v "\"POT-Creation" po/$$i > po.backup/$$i ; \
	  cp po/$$i po.backup/backup_$$i ;  \
	 touch -r po/$$i po.backup/backup_$$i; \
	done
	@for subdir in $(SUBDIRS); do \
	echo "Making messages in $$subdir"; \
	  (cd $$subdir && $(MAKE) messages) \
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
	(cd po && $(MAKE) merge)
