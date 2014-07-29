# defines a set of products required when only Words and Stage are wanted. Used for the Gemini project

calligra_define_productset(WORDS+STAGE "Builds the Stage and Words components, and Calligra for 2:1 devices"
    OPTIONAL
# apps
    WORDS
    STAGE 
# plugins
    PLUGIN_ARTISTICTEXTSHAPE
    PLUGIN_DOCKERS
    PLUGIN_TEXTEDITING
    PLUGIN_DEFAULTTOOLS
    PLUGIN_PATHSHAPES
    PLUGIN_VARIABLES
    PLUGIN_CHARTSHAPE
    PLUGIN_PICTURESHAPE
    PLUGIN_TEXTSHAPE
    PLUGIN_PLUGINSHAPE
    PLUGIN_FORMULASHAPE
    PLUGIN_VIDEOSHAPE
    PLUGIN_VECTORSHAPE
    PLUGIN_QTQUICK
    GEMINI
)
