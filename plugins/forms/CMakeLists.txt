# SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: LGPL-2.0-or-later

add_definitions(-DTRANSLATION_DOMAIN="calligra_shape_forms")

include_directories(${FLAKE_INCLUDES})

set(forms_LIB_SRCS
    KoFormTool.cpp
    KoFormShapeRenderer.cpp
    KoFormPdfExporter.cpp
    KoFormShape.cpp
    KoFormShapeFactory.cpp
    Plugin.cpp
)

add_library(calligra_shape_forms MODULE ${forms_LIB_SRCS})
target_link_libraries(calligra_shape_forms
    flake
    komain
    koodf
    KF6::I18n
)

if(TARGET qpdf::libqpdf)
    set_source_files_properties(KoFormPdfExporter.cpp PROPERTIES COMPILE_OPTIONS "-fexceptions")
    target_link_libraries(calligra_shape_forms qpdf::libqpdf)
    target_compile_definitions(calligra_shape_forms PRIVATE CALLIGRA_WITH_QPDF)
else()
    message(STATUS "QPDF not found: interactive PDF form export will be unavailable")
endif()

install(TARGETS calligra_shape_forms DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/shapes)

if(BUILD_TESTING)
    ecm_add_test(tests/TestFormRendering.cpp KoFormShapeRenderer.cpp KoFormShape.cpp
        TEST_NAME TestFormRendering
        NAME_PREFIX "plugins-forms-"
        LINK_LIBRARIES flake komain koodf KF6::I18n Qt6::Test
    )
    target_include_directories(TestFormRendering PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    if(TARGET qpdf::libqpdf)
        ecm_add_test(tests/TestFormPdfExport.cpp KoFormPdfExporter.cpp
            TEST_NAME plugins-forms-PdfExport
            LINK_LIBRARIES qpdf::libqpdf koodf KF6::Archive Qt6::Gui Qt6::Test)
        set_source_files_properties(tests/TestFormPdfExport.cpp KoFormPdfExporter.cpp PROPERTIES COMPILE_OPTIONS "-fexceptions")
        target_compile_definitions(plugins-forms-PdfExport PRIVATE CALLIGRA_WITH_QPDF)
    endif()
endif()
