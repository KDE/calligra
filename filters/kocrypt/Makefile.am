####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libkocryptimport_la_LDFLAGS	= -module -avoid-version -no-undefined
libkocryptimport_la_LIBADD = $(KOFFICE_LIBS)
libkocryptexport_la_LDFLAGS	= -module -avoid-version -no-undefined
libkocryptexport_la_LIBADD = $(KOFFICE_LIBS)

####### Files

kde_module_LTLIBRARIES = libkocryptimport.la libkocryptexport.la

libkocryptimport_la_SOURCES = kocryptimport.cc blowfish.cc cbc.cc blockcipher.cc hashbase.cc pwdprompt.cc sha1.cc keyutil.cc

libkocryptexport_la_SOURCES = kocryptexport.cc blowfish.cc cbc.cc blockcipher.cc hashbase.cc pwdprompt.cc sha1.cc keyutil.cc

noinst_HEADERS = kocryptimport.h kocryptexport.h  \
	cbc.h blowfish.h blockcipher.h            \
	blowfishtables.h hashbase.h pwdprompt.h sha1.h keyutil.h

METASOURCES = AUTO

service_DATA = kword_crypto_import.desktop kword_crypto_export.desktop    \
	kspread_crypto_import.desktop kspread_crypto_export.desktop

servicedir = $(kde_servicesdir)
