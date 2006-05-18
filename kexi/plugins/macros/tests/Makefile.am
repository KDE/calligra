if include_kunittestgui
  GUIBINPROGRAM = komacrotestgui
endif

bin_PROGRAMS = komacrotest $(GUIBINPROGRAM)

komacrotest_SOURCES = komacrotest.cpp testobject.cpp testaction.cpp actiontests.cpp macrotests.cpp
komacrotest_LDFLAGS = $(KDE_RPATH) $(all_libraries)
komacrotest_LDADD = -lkunittest ../lib/libkomacro.la $(LIB_KDEUI) $(LIB_KPARTS)

if include_kunittestgui
  komacrotestgui_SOURCES = komacrotestgui.cpp testobject.cpp testaction.cpp actiontests.cpp macrotests.cpp
  komacrotestgui_LDFLAGS = $(KDE_RPATH) $(all_libraries)
  komacrotestgui_LDADD = -lkunittestgui ../lib/libkomacro.la $(LIB_KDEUI) $(LIB_KPARTS)
endif

KDE_CXXFLAGS = $(USE_EXCEPTIONS)
INCLUDES = -I$(srcdir)/tests -I$(srcdir)../ $(all_includes)
METASOURCES = AUTO

guicheck: komacrotestgui
	kunittest ./komacrotestgui 

check: komacrotest
	echo $(srcdir)
	kunittest ./komacrotest
