
INCLUDES	=	-I../kofficecore -I$(KDEDIR)/include/idl $(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS		=	$(all_libraries) $(KDE_RPATH)
LDADD 		=  	-lkofficecore -lkofficeui -lkoml -lkom -lop -lkfile -lkfm -lkdecore -lkdeui $(LIBMICO) -lqt -lXext -lX11

bin_PROGRAMS = test
test_SOURCES = test.cc
DISTCLEANFILES = test.moc

test.cc: test.moc
test.moc: test.h
	$(MOC) -o test.moc test.h
