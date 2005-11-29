SUBDIRS = kohyphen . tests
INCLUDES= $(KOFFICECORE_INCLUDES) $(KOFFICEUI_INCLUDES) $(KSTORE_INCLUDES) $(all_includes)
## Needed for kohyphen...
KDE_CXXFLAGS = $(USE_EXCEPTIONS)

lib_LTLIBRARIES = libkotext.la
libkotext_la_SOURCES = qcomplextext.cpp korichtext.cpp \
	kotextzoomhandler.cpp koborder.cc \
	kotextformat.cc koparagcounter.cc kostyle.cc kocommand.cc \
	kotextparag.cc kotextdocument.cc kotextformatter.cc koparaglayout.cc \
	kotextobject.cc kotextview.cc koAutoFormat.cc koAutoFormatDia.cc \
	koParagDia.cc kovariable.cc koVariableDlgs.cc \
	koChangeCaseDia.cc koStylist.cc koSearchDia.cc \
	timedateformatwidget.ui timeformatwidget_impl.cc dateformatwidget_impl.cc \
	kfontdialog_local.cpp kofonttabbase.ui kofonttab.cc kohighlightingtabbase.ui kohighlightingtab.cc \
	kodecorationtabbase.ui kodecorationtab.cc kolayouttabbase.ui kolayouttab.cc kolanguagetabbase.ui kolanguagetab.cc \
	kofontdiapreview.cc \
	KoTextViewIface.skel KoTextViewIface.cc koFontDia.cc kobgspellcheck.cc \
	koCommentDia.cc kospell.cc koCreateStyleDia.cc kotextcustomitem.cc \
	koimportstyledia.cc kotextiterator.cc kooasiscontext.cc koliststylestack.cc \
	KoUserStyle.cpp KoUserStyleCollection.cpp KoParagStyle.cpp

libkotext_la_LDFLAGS = $(all_libraries) -version-info 3:0:0 -no-undefined
# We use kofficeui for koRect :)
libkotext_la_LIBADD = $(LIB_KOFFICEUI) kohyphen/libkohyphen.la $(LIBKSPELL2) -lkutils

METASOURCES = AUTO

# include_HEADERS = 
#	kotextzoomhandler.h kotextformat.h koparagcounter.h kostyle.h 
#	kotextdocument.h kobgspellcheck.h koCommentDia.h koCreateStyleDia.h
