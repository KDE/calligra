AM_CPPFLAGS = -DQT_NO_CAST_ASCII

INCLUDES  = $(KOFFICE_INCLUDES) -I$(interfacedir) -I$(srcdir)/.. $(all_includes)

# Please keep sorted
check_PROGRAMS = fxtest macros myspinboxtest

# noinst_HEADERS = 

METASOURCES = AUTO

LDADD = ../libgraphitepart.la

# Please keep sorted
fxtest_SOURCES = fxtest.cc
macros_SOURCES = macros.cc
myspinboxtest_SOURCES = myspinboxtest.cc
