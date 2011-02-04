macro_optional_add_subdirectory( converter )
macro_optional_add_subdirectory( thumbnail )
macro_optional_add_subdirectory( kthesaurus )
macro_optional_add_subdirectory( quickprint )
macro_optional_add_subdirectory( slidecompare )
if(SHOULD_BUILD_TABLES AND SHOULD_BUILD_WORDS)
    macro_optional_add_subdirectory( cstester )
endif(SHOULD_BUILD_TABLES AND SHOULD_BUILD_WORDS)

if(SHOULD_BUILD_CALLIGRA)
  macro_optional_add_subdirectory(calligra)
endif(SHOULD_BUILD_CALLIGRA)

macro_optional_find_package(Okular)
macro_log_feature(OKULAR_FOUND "Okular" "Okular ODP Plugin" "http://okular.kde.org/" FALSE "" "Required to build the Okular OpenDocument Presenter plugin")

if(OKULAR_FOUND)
    add_subdirectory( okularodpgenerator )
endif(OKULAR_FOUND)

macro_optional_find_package(X11)
macro_optional_find_package(Qt4)
macro_optional_find_package(LibXml2) # f-office uses xml2 directly
macro_log_feature(LIBXML2_FOUND "XML2" "A library for handling XML documents" "http://xmlsoft.org" FALSE "" "FreOffice uses XML2 for digital signatures")
macro_optional_find_package(OpenSSL) # f-office uses ssl directly
macro_log_feature(OPENSSL_FOUND "OpenSSL" "A toolkit implementing the Secure Sockets Layer (SSL v2/v3) and Transport Layer Security (TLS v1) protocols" "http://openssl.org" FALSE "" 
"FreOffice uses OpenSSL for digital signatures")


# TODO why build koabstraction and f-office only on X11?
if(X11_FOUND AND LIBXML2_FOUND AND OPENSSL_FOUND)
    if(Q_WS_X11 AND SHOULD_BUILD_TABLES AND SHOULD_BUILD_WORDS AND SHOULD_BUILD_KPRESENTER)
      macro_optional_add_subdirectory( koabstraction )
      macro_optional_add_subdirectory( f-office )
    endif(Q_WS_X11 AND SHOULD_BUILD_TABLES AND SHOULD_BUILD_WORDS AND SHOULD_BUILD_KPRESENTER)
endif(X11_FOUND AND LIBXML2_FOUND AND OPENSSL_FOUND)

# strigi ships with the odfsaxanalyzer which does already fetch the meta-data of
# odf files. Also the current implementations of the analyzers are deactivated
# anyway. So, what would be needed is to connect somehow with KoStore or hook
# in the koffice-filters another way somehow.
#add_subdirectory( strigi-analyzer )
