bin_PROGRAMS = kexisetup

kexisetup_SOURCES = kexisetupwizard_skel.ui kexisetupwizard.cpp main.cpp

kexisetup_LDADD   = $(LIB_KDECORE) $(LIB_KDEUI)
 
SUBDIRS = .


METASOURCES = AUTO

INCLUDES= $(all_includes)


# the library search path. 
kexisetup_LDFLAGS = $(all_libraries) $(KDE_RPATH)


#Should already be handled by the toplevel Makefile
#messages: rc.cpp
#	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
#	if test -n "$$LIST"; then \
#	 $(XGETTEXT) $$LIST -o $(podir)/kexi.pot; \
#	fi

