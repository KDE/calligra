
INCLUDES	=	$(KOFFICECORE_INCLUDES) $(KOFFICEUI_INCLUDES) $(all_includes) $(MICO_INCLUDES) -I$(idldir)
LDFLAGS		=	$(all_libraries) $(KDE_RPATH)
LDADD 		=  	../libkofficeui.la ../../kofficecore/libkofficecore.la ../../koml/libkoml.la -lkstore -lkom -lkpartsui -lkparts $(LIB_KDEUTIL) $(LIBMICO)     $(LIB_KFILE) 

check_PROGRAMS = test
test_SOURCES = test.cc
METASOURCES = test.moc

