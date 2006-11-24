




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

