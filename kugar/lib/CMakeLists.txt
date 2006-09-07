




########### next target ###############

set(kugarlib_LIB_SRCS 
   mcalcobject.cpp 
   mfieldobject.cpp 
   mlabelobject.cpp 
   mlineobject.cpp 
   mpagecollection.cpp 
   mpagedisplay.cpp 
   mreportdetail.cpp 
   mreportengine.cpp 
   mreportobject.cpp 
   mreportsection.cpp 
   mreportviewer.cpp 
   mspecialobject.cpp 
   mutil.cpp 
   inputmask.cpp )

kde4_automoc(${kugarlib_LIB_SRCS})

kde4_add_library(kugarlib SHARED ${kugarlib_LIB_SRCS})

target_link_libraries(kugarlib  ${KDE4_KDEPRINT_LIBS})

set_target_properties(kugarlib PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kugarlib  DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############






#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) $(all_includes)
#
#METASOURCES = AUTO
#
#libkugarlib_la_LIBADD = $(LIB_QT) $(LIB_KDEPRINT)
#
#libkugarlib_la_LDFLAGS = $(all_libraries) -version-info 1:2 -no-undefined
#
## libkugar.la is already reserved for the kdeinit module for kugar (on some platforms),
## so we have to call the lib differently, e.g. libkugarlib
#lib_LTLIBRARIES = libkugarlib.la
#
#libkugarlib_la_SOURCES = mcalcobject.cpp mfieldobject.cpp mlabelobject.cpp \
#	mlineobject.cpp mpagecollection.cpp mpagedisplay.cpp mreportdetail.cpp \
#	mreportengine.cpp mreportobject.cpp mreportsection.cpp mreportviewer.cpp \
#	mspecialobject.cpp mutil.cpp inputmask.cpp
#
#include_HEADERS =
#noinst_HEADERS = inputmask.h
