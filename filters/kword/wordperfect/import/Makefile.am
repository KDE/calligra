INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(LIBWPD_CFLAGS) $(all_includes)

kde_module_LTLIBRARIES = libwpimport.la 

libwpimport_la_SOURCES = wpimport.cc DocumentElement.cxx FontStyle.cxx ListStyle.cxx PageSpan.cxx SectionStyle.cxx TableStyle.cxx TextRunStyle.cxx WordPerfectCollector.cxx
libwpimport_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
libwpimport_la_LIBADD = $(KOFFICE_LIBS) $(LIBWPD_LIBS)

noinst_HEADERS = wpimport.h FilterInternal.hxx Style.hxx WriterProperties.hxx DocumentElement.hxx DocumentHandler.hxx FontStyle.hxx ListStyle.hxx PageSpan.hxx SectionStyle.hxx TableStyle.hxx TextRunStyle.hxx WordPerfectCollector.hxx


METASOURCES = AUTO

service_DATA = kword_wp_import.desktop 
servicedir = $(kde_servicesdir)
