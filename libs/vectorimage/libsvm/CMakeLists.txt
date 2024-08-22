

include_directories( ${KOMAIN_INCLUDES} )

#add_subdirectory(demo)


########### library target ###############

set(libsvm_LIB_SRCS 
     #SvmStructs.cpp
)

#add_library(libsvm SHARED ${libsvm_LIB_SRCS})

#target_link_libraries(libsvm   Qt6::Gui )

#set_target_properties(libsvm PROPERTIES VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION} )


########### install files ###############

#install(TARGETS libsvm  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

#install(
#    FILES
#    SvmEnums.h
#    SvmStructs.h
#    svm_export.h
#    DESTINATION ${KDE_INSTALL_INCLUDEDIR} COMPONENT Devel
#)
