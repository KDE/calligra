INCLUDES = -I$(top_srcdir)/core \
	-I$(top_srcdir)/kspread $(KOFFICECORE_INCLUDES) $(KSTORE_INCLUDES) \
	$(KOFFICEUI_INCLUDES) $(KROSS_INCLUDES) \
        $(all_includes)

kde_module_LTLIBRARIES = krosskspreadcore.la

krosskspreadcore_la_SOURCES = kspreadcoremodule.cpp krs_doc.cpp krs_sheet.cpp krs_cell.cpp krs_color.cpp

krosskspreadcore_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) -module
krosskspreadcore_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    $(top_builddir)/lib/kross/api/libkrossapi.la \
    $(top_builddir)/lib/kross/main/libkrossmain.la \
    $(top_builddir)/kspread/libkspreadcommon.la

METASOURCES = AUTO
SUBDIRS = .

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

noinst_HEADERS = krs_doc.h krs_sheet.h krs_cell.h krs_color.h
