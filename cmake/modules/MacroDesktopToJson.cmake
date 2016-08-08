# Convenience macro wrappers around kcoreaddons_desktop_to_json

# Copyright (c) 2016 Friedrich W. H. Kossebau <kossebau@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

function(calligra_part_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/servicetypes/calligra_part.desktop
    )
endif()

endfunction()


function(calligra_filter_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/servicetypes/calligra_filter.desktop
    )
endif()

endfunction()


function(calligra_shape_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/libs/flake/flakeshape.desktop
    )
endif()

endfunction()


function(calligra_docker_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/libs/main/calligradocker.desktop
    )
endif()

endfunction()


function(calligra_deferredplugin_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/servicetypes/calligra_deferred_plugin.desktop
    )
endif()

endfunction()


function(calligra_tool_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/libs/flake/flaketool.desktop
    )
endif()

endfunction()


function(calligra_device_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/libs/flake/flakedevice.desktop
    )
endif()

endfunction()


function(calligra_textinlineobject_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/libs/text/inlinetextobject.desktop
    )
endif()

endfunction()


function(calligra_texteditingplugin_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/libs/text/texteditingplugin.desktop
    )
endif()

endfunction()


function(calligra_presentationeventaction_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/libs/flake/presentationeventaction.desktop
    )
endif()

endfunction()


function(calligra_filtereffect_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/libs/flake/filtereffect.desktop
    )
endif()

endfunction()


function(calligra_colorspace_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/libs/pigment/pigment.desktop
    )
endif()

endfunction()


function(calligra_pageapptool_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/libs/pageapp/tools/kopa_tool.desktop
    )
endif()

endfunction()


function(karbon_viewplugin_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/karbon/data/karbon_viewplugin.desktop
    )
endif()

endfunction()


function(calligrasheets_plugin_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/sheets/sheets_plugin.desktop
    )
endif()

endfunction()


function(calligrasheets_viewplugin_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/sheets/sheets_viewplugin.desktop
    )
endif()

endfunction()


function(calligrastage_pageeffect_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/stage/data/kpr_pageeffect.desktop
    )
endif()

endfunction()


function(calligrastage_tool_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/stage/data/kpr_tool.desktop
    )
endif()

endfunction()


function(calligrastage_shapeanimation_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/stage/data/kpr_shapeanimation.desktop
    )
endif()

endfunction()


function(braindump_extensions_desktop_to_json target desktop)

if(${KF5_VERSION} VERSION_LESS "5.16.0")
    kcoreaddons_desktop_to_json(${target} ${desktop})
else()
    kcoreaddons_desktop_to_json(${target} ${desktop}
        SERVICE_TYPES ${CMAKE_SOURCE_DIR}/braindump/data/braindump_extensions.desktop
    )
endif()

endfunction()
