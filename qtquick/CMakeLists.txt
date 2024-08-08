project(calligraqtquickcomponents)

# While it is not standard practice to make use of this, we have to do so here:
# Qt Quick components have to be located in some discoverable location, known
# as the Qt Quick Components Import directory. This can be any number of
# locations, but most importantly it must be inside the library location, as
# they tend to be binary plugins, like this one is. As it turns out, GNUInstallDirs
# is broken in some linux distributions, but we must consider this a bug in the
# distribution, which needs to be fixed.
#
# An example (based on this plugin) of how Qt Quick locates modules:
# A qml file requires a module by having the statement
# import org.calligra.CalligraComponents 1.0
# and this will cause Qt Quick to look through its list of import locations for
# a directory structure matching
# (importdir)/org/calligra/CalligraComponents/qmldir
# The qmldir file is important, and defines the module as one such (see Qt
# documentation for details on how to construct one). For us, it defines
# specifically the module's library file with the like
# plugin calligraqtquickcomponentsplugin
# which makes Qt Quick look in the same directory (and only there) for a file
# which matches that description. On various Unixes this would be
# libcalligratquickcomponents.so
# and on Windows it is
# calligraqtquickcomponents.dll
#
# So, in short - because this is binary, executable data, we can't put it in e.g.
# the data dir (for which we would not need GNUInstallDirs), and as it is a library
# we also cannot put it in the binaries dir. In short, it must be the library
# location, and without GNUInstallDirs, this turns out to be very difficult/awkward
#
# Personal note:
# It is perhaps also worth mentioning that this has been gone over several times
# by various people, and changing the method keeps breaking in various places, and
# in particular on Windows, which as always is a second class citizen (unsurprisingly,
# but it simply means it doesn't get tested as rigorously, and wellmeaning changes
# such as install location changes have wide reaching consequences for these less
# well supported targets).
# This would be worth revisiting for the Frameworks 5 port, where i believe certain
# changes have been made to the cmake system, to accommodate the Qt Quick plugins
# built for Plasma 5. At that time, we can switch to using the same installation
# methodology as is used there. Until then, i would prefer to not break things
# any further by attempting to "fix" this by not using GNUInstallDirs and switching
# back to using something which doesn't work on Windows.
include(GNUInstallDirs)

set(calligraqtquickcomponents_SRCS
    CQThumbnailItem.cpp
    CQPresentationModel.cpp
#    CQSpreadsheetListModel.cpp
    CQCanvasBase.cpp
    CQCanvasControllerItem.cpp
#    CQSpreadsheetCanvas.cpp
    CalligraQtQuickComponentsPlugin.cpp
    CQCanvasController.cpp
    CQTextDocumentCanvas.cpp
    CQTextDocumentModel.cpp
    CQImageProvider.cpp
    CQPresentationView.cpp
    CQPresentationCanvas.cpp
    CQTextToCModel.cpp
    CQPluginLoaderProxy.cpp
    CQLinkArea.cpp
    CQTextDocumentNotesModel.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/words/part
#    ${CMAKE_SOURCE_DIR}/sheets
    ${CMAKE_SOURCE_DIR}/libs/pageapp
)

add_library(calligraqtquickcomponentsplugin SHARED ${calligraqtquickcomponents_SRCS})

target_link_libraries(calligraqtquickcomponentsplugin
    Qt6::Quick
    Qt6::OpenGL
    Qt6::Svg
    komain
    kotextlayout
    wordsprivate
#    calligrasheetsui
    kopageapp
    koplugin
    calligrastageprivate)

install(TARGETS calligraqtquickcomponentsplugin
    DESTINATION ${KDE_INSTALL_QMLDIR}/org/calligra/CalligraComponents)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/calligra/CalligraComponents)
