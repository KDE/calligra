# it does not compile yet, so here is what to do for now..
# move this file to call it Makefile.am
# do a make -f Makefile.cvs at toplevel
# do a configure 
# and you are ready to go!

INCLUDES = $(all_includes)

bin_PROGRAMS = kplato
kplato_SOURCES =  kptmilestone.cc kptnode.cc kptproject.cc \
                kptrelation.cc kptresource.cc kpttask.cc \
                main.cc
kplato_LDADD =  $(all_libraries) $(LIB_KDECORE)
