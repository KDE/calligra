find_package(Strigi REQUIRED)
macro_log_feature(STRIGI_FOUND "Strigi" "Library for analyzing the contents of files for the Strigi search engine" "http://www.kde.org" FALSE "" "Required by the Strigi Analyzer")

add_subdirectory( koffice )

# http://websvn.kde.org/trunk/koffice/tools/strigi-analyzer/gnumeric/gnumericthroughanalyzer.cpp?revision=688925&view=markup
#add_subdirectory( gnumeric )

# http://websvn.kde.org/trunk/koffice/tools/strigi-analyzer/abiword/abiwordthroughanalyzer.cpp?revision=694641&view=markup
#add_subdirectory( abiword )
