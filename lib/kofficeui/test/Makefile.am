
INCLUDES	=	-I../../kofficecore -I.. $(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS		=	$(all_libraries) $(KDE_RPATH)
LDADD 		=  	../libkofficeui.la ../../kofficecore/libkofficecore.la ../../koml/libkoml.la -lkstore -lkom -lkpartsui -lkparts   -lkdeutil   $(LIBMICO)     $(LIB_KFILE) 

check_PROGRAMS = test
test_SOURCES = test.cc
DISTCLEANFILES = test.moc

test.cc: test.moc
test.moc: test.h
	$(MOC) -o test.moc test.h
