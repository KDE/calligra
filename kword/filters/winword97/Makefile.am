
INCLUDES = $(KOFFICE_INCLUDES) $(KOFFICECORE_INCLUDES) \
	$(all_includes) $(MICO_INCLUDES) 
LDFLAGS	= $(all_libraries) $(MICO_LDFLAGS) $(KDE_RPATH)
LDADD = $(LIB_KOFFICECORE)

####### Files

bin_PROGRAMS = kword_winword97

kword_winword97_SOURCES = main.cc wordfilter.cc winworddoc.cc \
kworddoc.cc

noinst_HEADERS = main.h wordfilter.h winworddoc.h kworddoc.h \
anld.h anlv.h asumy.h asumyi.h atrd.h bkd.h bkf.h bkl.h brc.h \
brc10.h bte.h chp.h chpx.h chpx_fkp.h dcs.h dogrid.h doptypography.h \
dttm.h ffn.h fib.h fld.h frd.h fspa.h ftxbxs.h lfo.h lfolvl.h \
lspd.h lstf.h lvlf.h myfile.h numrm.h olst.h pap.h papx.h \
papx_fkp.h pcd.h pgd.h phe.h picf.h plcf.h prm.h rr.h rs.h \
sed.h sep.h sepx.h shd.h tap.h tbd.h tc.h tlp.h wkb.h

METASOURCES = main.moc

service_DATA = kword_winword97_import.desktop
servicedir = $(kde_servicesdir)

datasdir = $(kde_datadir)/koffice/filter

data_DATA = winword97.desktop
datadir = $(kde_mimedir)/application

#bin_SCRIPTS = kword_winword97

####### Build rules

messages:
#	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot  *.cc && mv messages.po ../../kspread_filter.pot

main.cc: main.h
