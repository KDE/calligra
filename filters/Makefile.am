
if compile_filter_KARBON
KARBONDIR = karbon
endif

if include_XSLTFILTERS
XSLTFILTERDIR = xsltfilter
endif

if compile_filter_KWORD
KWORDDIR = kword
endif

if compile_filter_KPRESENTER
KPRESENTERDIR = kpresenter
endif

if compile_filter_KSPREAD
KSPREADDIR = kspread
endif

if compile_filter_KCHART
KCHARTDIR = kchart
endif

if compile_filter_KFORMULA
KFORMULADIR = kformula
endif

if compile_filter_KUGAR
KUGARDIR = kugar
endif

if compile_filter_KRITA
KRITADIR = krita
endif


SUBDIRS = generic_wrapper liboofilter $(KSPREADDIR) $(KCHARTDIR) $(KWORDDIR) $(KPRESENTERDIR) olefilters $(XSLTFILTERDIR) $(KFORMULADIR) $(KARBONDIR) $(KUGARDIR) $(KRITADIR)

messages:
	$(EXTRACTRC) `find . -name \*.ui` > rc.cpp
	$(XGETTEXT) `find . -name \*.cpp -o -name \*.cc` -o $(podir)/kofficefilters.pot
