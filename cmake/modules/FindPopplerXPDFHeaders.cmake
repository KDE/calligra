# - Try to find the poppler not-officially-supported XPDF headers
# Once done this will define
#
#  PopplerXPDFHeaders_FOUND - system has poppler not-officially-supported XPDF headers
#

find_package(Poppler COMPONENTS Core)

if(Poppler_Core_FOUND)
    find_path(POPPLER_XPDF_HEADERS poppler-config.h
        HINTS ${Poppler_INCLUDE_DIRS}
    )

    if(POPPLER_XPDF_HEADERS)
        set(PopplerXPDFHeaders_FOUND TRUE)
    endif()
endif()
