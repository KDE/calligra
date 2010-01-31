include_directories( ${KOMAIN_INCLUDES} )

########### next target ###############

set(kwordexportfilters_LIB_SRCS
    TagProcessing.cc
    KWEFStructures.cc
    ProcessDocument.cc
    KWEFUtil.cc
    KWEFKWordLeader.cc
    KWEFBaseWorker.cc
    Picture.h
    PictureBase.h
    PictureClipart.h
    PictureEps.h
    PictureImage.h
    PictureKey.h
    PictureShared.h
    Picture.cpp
    PictureBase.cpp
    PictureClipart.cpp
    PictureEps.cpp
    PictureImage.cpp
    PictureKey.cpp
    PictureShared.cpp
)

kde4_add_library(kwordexportfilters SHARED ${kwordexportfilters_LIB_SRCS})

target_link_libraries(kwordexportfilters komain)
target_link_libraries(kwordexportfilters LINK_INTERFACE_LIBRARIES komain)

set_target_properties(kwordexportfilters PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kwordexportfilters  ${INSTALL_TARGETS_DEFAULT_ARGS})

