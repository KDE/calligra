
INCLUDES	=	-I../kofficeui -I../kofficecore $(all_includes)
LDFLAGS		=	$(all_libraries)
LDADD 		=  	../kofficeui/libkofficeui.la \
	../kofficecore/libkofficecore.la \
	../koml/libkoml.la -lkom -lop -lkfile -lkfm  -lkdeui \
	-lkdecore $(LIBMICO) -lqt -lXext -lX11

check_PROGRAMS = test
test_SOURCES = test.cc
DISTCLEANFILES = test.moc

test.cc: test.moc
test.moc: test.h
	$(MOC) -o test.moc test.h
