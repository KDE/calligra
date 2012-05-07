include_directories( ${KOMAIN_INCLUDES} )

########### next target ###############

set(wordsexportfilters_LIB_SRCS
    TagProcessing.cc
    KWEFStructures.cc
    ProcessDocument.cc
    KWEFUtil.cc
    KWEFWordsLeader.cc
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

kde4_add_library(wordsexportfilters SHARED ${wordsexportfilters_LIB_SRCS})

target_link_libraries(wordsexportfilters ${KOMAIN_LIBRARY})
target_link_libraries(wordsexportfilters LINK_INTERFACE_LIBRARIES ${KOMAIN_LIBRARY})

set_target_properties(wordsexportfilters PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )
install(TARGETS wordsexportfilters  ${INSTALL_TARGETS_DEFAULT_ARGS})

