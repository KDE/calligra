####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
libkocryptimport_la_LDFLAGS	= -module -avoid-version -no-undefined
libkocryptimport_la_LIBADD = $(KOFFICE_LIBS)
libkocryptexport_la_LDFLAGS	= -module -avoid-version -no-undefined
libkocryptexport_la_LIBADD = $(KOFFICE_LIBS)

####### Files

kde_module_LTLIBRARIES = libkocryptimport.la libkocryptexport.la

libkocryptimport_la_SOURCES = kocryptimport.cc kocryptimport_factory.cc blowfish.cc cbc.cc blockcipher.cc hashbase.cc pwdprompt.cc sha1.cc

libkocryptexport_la_SOURCES = kocryptexport.cc kocryptexport_factory.cc blowfish.cc cbc.cc blockcipher.cc hashbase.cc pwdprompt.cc sha1.cc

noinst_HEADERS = kocryptimport.h kocryptimport_factory.h kocryptexport.h  \
	kocryptexport_factory.h cbc.h blowfish.h blockcipher.h            \
	blowfishtables.h hashbase.h pwdprompt.h sha1.h

METASOURCES = AUTO

mimelnk_DATA = x-kword-crypt.desktop x-kspread-crypt.desktop
mimelnkdir = $(kde_mimedir)/application

service_DATA = kword_crypto_import.desktop kword_crypto_export.desktop    \
	kspread_crypto_import.desktop kspread_crypto_export.desktop

servicedir = $(kde_servicesdir)

