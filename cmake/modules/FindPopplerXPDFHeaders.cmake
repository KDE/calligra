# - Try to find the poppler not-officially-supported XPDF headers
# Once done this will define
#
#  PopplerXPDFHeaders_FOUND - system has poppler not-officially-supported XPDF headers
#

find_package(Poppler)

if(POPPLER_FOUND)
    find_path(POPPLER_XPDF_HEADERS poppler-config.h
        HINTS ${POPPLER_INCLUDE_DIR}
    )

    if(POPPLER_XPDF_HEADERS)
        set(PopplerXPDFHeaders_FOUND TRUE)
    endif()
endif()
