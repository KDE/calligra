
if ! do_not_compile_KARBON
if include_KARBON
KARBONDIR = karbon
endif
endif

if ! do_not_compile_KONTOUR
if include_KARBON
KONTOURDIR = kontour
endif
endif

if include_XSLTFILTERS
XSLTFILTERDIR = xsltfilter
endif

if ! do_not_compile_KWORD
KWORDDIR = kword
endif

if ! do_not_compile_KPRESENTER
KPRESENTERDIR = kpresenter
endif

if ! do_not_compile_KSPREAD
KSPREADDIR = kspread
endif

SUBDIRS = generic_wrapper $(KSPREADDIR) $(KWORDDIR) $(KPRESENTERDIR) olefilters $(KONTOURDIR) $(XSLTFILTERDIR) kformula $(KARBONDIR)
