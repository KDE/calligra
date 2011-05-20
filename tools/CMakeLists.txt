macro_optional_add_subdirectory( converter )
macro_optional_add_subdirectory( thumbnail )
macro_optional_add_subdirectory( kthesaurus )
macro_optional_add_subdirectory( quickprint )
macro_optional_add_subdirectory( slidecompare )
if(SHOULD_BUILD_CSTESTER)
    macro_optional_add_subdirectory( cstester )
endif(SHOULD_BUILD_CSTESTER)

if(SHOULD_BUILD_CALLIGRA)
  macro_optional_add_subdirectory(calligra)
endif(SHOULD_BUILD_CALLIGRA)

macro_optional_find_package(Okular)
macro_log_feature(OKULAR_FOUND "Okular" "Okular ODP Plugin" "http://okular.kde.org/" FALSE "" "Required to build the Okular OpenDocument Presenter plugin")

if(OKULAR_FOUND)
    add_subdirectory( okularodpgenerator )
endif(OKULAR_FOUND)

add_subdirectory(scripts)

macro_optional_find_package(X11)
macro_optional_find_package(Qt4)
macro_optional_find_package(LibXml2) # calligra mobile uses xml2 directly
macro_log_feature(LIBXML2_FOUND "XML2" "A library for handling XML documents" "http://xmlsoft.org" FALSE "" "Calligra Mobile uses XML2 for digital signatures")
macro_optional_find_package(OpenSSL) # calligra mobile uses ssl directly
macro_log_feature(OPENSSL_FOUND "OpenSSL" "A toolkit implementing the Secure Sockets Layer (SSL v2/v3) and Transport Layer Security (TLS v1) protocols" "http://openssl.org" FALSE "" 
"Calligra Mobile uses OpenSSL for digital signatures")

# TODO why build koabstraction and f-office only on X11?
if(X11_FOUND AND LIBXML2_FOUND AND OPENSSL_FOUND)
    if(SHOULD_BUILD_KOABSTRACTION)
        macro_optional_add_subdirectory(koabstraction)
    endif(SHOULD_BUILD_KOABSTRACTION)
    if(Q_WS_X11 AND SHOULD_BUILD_MOBILE)
        macro_optional_add_subdirectory(mobile)
    endif(Q_WS_X11 AND SHOULD_BUILD_MOBILE)
endif(X11_FOUND AND LIBXML2_FOUND AND OPENSSL_FOUND)
