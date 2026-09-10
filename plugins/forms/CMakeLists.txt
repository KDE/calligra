# SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: LGPL-2.0-or-later

add_definitions(-DTRANSLATION_DOMAIN="calligra_shape_forms")

include_directories(${FLAKE_INCLUDES})

set(forms_LIB_SRCS
    KoFormTool.cpp
    KoFormShapeRenderer.cpp
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

install(TARGETS calligra_shape_forms DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligra/shapes)

if(BUILD_TESTING)
    ecm_add_test(tests/TestFormRendering.cpp KoFormShapeRenderer.cpp
        TEST_NAME TestFormRendering
        NAME_PREFIX "plugins-forms-"
        LINK_LIBRARIES flake koodf KF6::I18n Qt6::Test
    )
    target_include_directories(TestFormRendering PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
