####### General stuff

INCLUDES = -I$(srcdir) -I$(srcdir)/../lib $(all_includes)
LDFLAGS = $(all_libraries)

####### Files

noinst_LTLIBRARIES = libwinword97.la

libwinword97_la_SOURCES = wordfilter.cc winworddoc.cc \
paragraph.cc picture.cc style.cc stylesheet.cc misc.cc \
section.cc msword.cc mswordgenerated.cc

noinst_HEADERS = winworddoc.h fib.h wordfilter.h \
paragraph.h picture.h style.h stylesheet.h misc.h \
section.h atrd.h sttbf.h bkl.h bkf.h ffn.h frd.h \
anld.h anlv.h asumy.h asumyi.h bkd.h brc.h brc10.h \
bte.h chp.h chpx.h chpx_fkp.h dcs.h dogrid.h \
doptypography.h dttm.h fld.h fspa.h ftxbxs.h lfo.h \
lfolvl.h lspd.h lstf.h lvlf.h numrm.h olst.h pap.h \
papx.h papx_fkp.h pgd.h phe.h picf.h plcf.h prm.h \
rr.h rs.h sed.h sep.h sepx.h shd.h tap.h tbd.h tc.h \
tlp.h wkb.h msword.h mswordgenerated.h

METASOURCES = AUTO

service_DATA = ole_winword97_import.desktop
servicedir = $(kde_servicesdir)

data_DATA = x-winword97.desktop
datadir = $(kde_mimedir)/application
