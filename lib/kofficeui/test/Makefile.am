
INCLUDES	=	-I../../kofficecore -I.. $(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS		=	$(all_libraries) $(KDE_RPATH)
LDADD 		=  	../libkofficeui.la ../../kofficecore/libkofficecore.la ../../koml/libkoml.la -lkstore -lkom -lkpartsui -lkparts -lkfile -lkfm -lkdeutil   $(LIBMICO)     $(LIB_KDEUI) 

check_PROGRAMS = test
test_SOURCES = test.cc
DISTCLEANFILES = test.moc

test.cc: test.moc
test.moc: test.h
	$(MOC) -o test.moc test.h
