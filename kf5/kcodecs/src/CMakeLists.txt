
set(kcodecs_SRCS
    kcharsets.cpp
    kcodecs.cpp
    kencodingprober.cpp
    probers/CharDistribution.cpp
    probers/ChineseGroupProber.cpp
    probers/JapaneseGroupProber.cpp
    probers/JpCntx.cpp
    probers/LangBulgarianModel.cpp
    probers/LangCyrillicModel.cpp
    probers/LangGreekModel.cpp
    probers/LangHebrewModel.cpp
    probers/LangHungarianModel.cpp
    probers/LangThaiModel.cpp
    probers/UnicodeGroupProber.cpp
    probers/nsBig5Prober.cpp
    probers/nsCharSetProber.cpp
    probers/nsEUCJPProber.cpp
    probers/nsEUCKRProber.cpp
    probers/nsEUCTWProber.cpp
    probers/nsEscCharsetProber.cpp
    probers/nsEscSM.cpp
    probers/nsGB2312Prober.cpp
    probers/nsHebrewProber.cpp
    probers/nsLatin1Prober.cpp
    probers/nsMBCSGroupProber.cpp
    probers/nsMBCSSM.cpp
    probers/nsSBCSGroupProber.cpp
    probers/nsSBCharSetProber.cpp
    probers/nsSJISProber.cpp
    probers/nsUniversalDetector.cpp
)

add_library(KCodecs ${kcodecs_SRCS})
generate_export_header(KCodecs)

target_link_libraries(KCodecs LINK_PUBLIC Qt5::Core )

set_target_properties(KCodecs PROPERTIES VERSION   ${KCODECS_VERSION_STRING}
                                         SOVERSION ${KCODECS_SOVERSION}
)

install(TARGETS KCodecs EXPORT KCodecsTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    kcharsets.h
    kcodecs.h
    kencodingprober.h
    ${CMAKE_CURRENT_BINARY_DIR}/kcodecs_export.h
    DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel
)
