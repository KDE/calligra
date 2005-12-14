
INCLUDES  = $(KOFFICE_INCLUDES) $(all_includes)

## The part
kde_module_LTLIBRARIES = libexamplepart.la
libexamplepart_la_SOURCES = example_part.cc \
	example_view.cc example_factory.cc
libexamplepart_la_LDFLAGS = $(KDE_PLUGIN)
libexamplepart_la_LIBADD = $(LIB_KOFFICEUI)

## The kdeinit loadable module and executable
kdeinit_LTLIBRARIES = example.la
lib_LTLIBRARIES =
bin_PROGRAMS =
example_la_SOURCES = main.cc
example_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
example_la_LIBADD = $(LIB_KOFFICEUI)

METASOURCES = AUTO

kdelnk_DATA = example.desktop
kdelnkdir = $(kde_appsdir)/Office

# Note: for your application, the mime type should be defined in
#       kdelibs CVS HEAD and a compatibility entry should be added in
#       the directory koffice/mimetypes/kde??
#       where ?? is the version number of the last stable kde(libs) release.
kdemime_DATA = x-vnd.kde.example.desktop
kdemimedir = $(kde_mimedir)/application

rc_DATA = example.rc example_readonly.rc
rcdir = $(kde_datadir)/example

messages: rc.cpp
	$(XGETTEXT) rc.cpp *.cc example_aboutdata.h -o $(podir)/example.pot
kde_services_DATA = examplepart.desktop

include $(top_srcdir)/admin/Doxyfile.am
