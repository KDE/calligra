####### General stuff

INCLUDES=	-I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../../kword \
$(all_includes) $(MICO_INCLUDES)
LDFLAGS	=	$(all_libraries) -version-info 1:0:0

####### Files

lib_LTLIBRARIES = libwinword97.la

libwinword97_la_SOURCES = wordfilter.cc winworddoc.cc \
kworddoc.cc filterbase.cc paragraph.cc picture.cc style.cc

noinst_HEADERS = winworddoc.h kworddoc.h anld.h anlv.h asumy.h \
asumyi.h atrd.h bkd.h bkf.h bkl.h brc.h brc10.h bte.h chp.h \
chpx.h chpx_fkp.h dcs.h dogrid.h doptypography.h dttm.h ffn.h \
fib.h fld.h frd.h fspa.h ftxbxs.h lfo.h lfolvl.h lspd.h lstf.h \
lvlf.h myfile.h numrm.h olst.h pap.h papx.h papx_fkp.h pcd.h \
pgd.h phe.h picf.h plcf.h prm.h rr.h rs.h sed.h sep.h sepx.h \
shd.h tap.h tbd.h tc.h tlp.h wkb.h wordfilter.h filterbase.h \
paragraph.h picture.h style.h

METASOURCES = wordfilter.moc filterbase.moc

service_DATA = ole_winword97_import.desktop
servicedir = $(kde_servicesdir)

data_DATA = x-winword97.desktop
datadir = $(kde_mimedir)/application
