
SUBDIRS = lib kspread kpresenter kword kformula kdiagramm killustrator kimage kohtml tools plugins koshell pics

AUTOMAKE_OPTIONS = foreign # 1.4

acinclude.m4: admin/acinclude.m4.in admin/libtool.m4.in
	@cat $(top_srcdir)/admin/acinclude.m4.in $(top_srcdir)/admin/libtool.m4.in   > $(top_srcdir)/acinclude.m4

messages:
	make -f admin/Makefile.common package-messages

dist-hook:
	cd $(top_distdir) && perl $(top_srcdir)/admin/am_edit -padmin

