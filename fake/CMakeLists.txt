find_package(ZLIB)
set_package_properties(ZLIB PROPERTIES DESCRIPTION "Support for gzip compressed files and data streams"
                       URL "http://www.zlib.net"
                       TYPE REQUIRED
                       PURPOSE "Required by the core KDE libraries and some critical kioslaves"
                      )


set(kofake_LIB_SRCS
    karchive.cpp
    kcomponentdata.cpp
    kconfigbase.cpp
    kfilterbase.cpp
    kfilterdev.cpp
    kgzipfilter.cpp
    klimitediodevice.cpp
    kpluginfactory.cpp
    ktar.cpp
    kzip.cpp
    kio/job.cpp
    kaction.cpp
    kdialog.cpp
    kocursor.cpp
)

set(kofake_HEADERS 
config-compression.h
config.h
kaboutdata.h
kactioncollection.h
kactionmenu.h
kapplication.h
kofake_export.h
karchive.h
kcharsets.h
kcmdlineargs.h
kcolorbutton.h
kcolordialog.h
kcolormimedata.h
kcolorscheme.h
kcolorutils.h
kcombobox.h
kcomponentdata.h
kconfigbase.h
kconfiggroup.h
kconfig.h
kdatetime.h
kdebug.h
kofake_export.h
kde_file.h
kdemacros.h
kdeprintdialog.h
kdesktopfile.h
kdeversion.h
kdiroperator.h
kedittoolbar.h
kemailsettings.h
kfiledialog.h
kfilefiltercombo.h
kfile.h
kfileitem.h
kfilewidget.h
kfilterbase.h
kfilterdev.h
kfinddialog.h
kfind.h
kglobal.h
kglobalsettings.h
kguiitem.h
kgzipfilter.h
khistorycombobox.h
kicondialog.h
kicon.h
kiconloader.h
kimageio.h
kinputdialog.h
klibloader.h
klimitediodevice_p.h
klocale.h
klocalizedstring.h
kmainwindow.h
kmessagebox.h
kmimetype.h
knotification.h
knuminput.h
kpagedialog.h
kpagewidget.h
kpagewidgetitem.h
kpagewidgetmodel.h
kpluginloader.h
kpropertiesdialog.h
kpushbutton.h
krecentdirs.h
krecentdocument.h
krecentfilesaction.h
kreplacedialog.h
kselectaction.h
kselector.h
kservice.h
kservicetype.h
kservicetypetrader.h
ksharedconfig.h
ksharedptr.h
kshortcut.h
ksqueezedtextlabel.h
kstandardaction.h
kstandarddirs.h
kstandardshortcut.h
kstatusbar.h
kstringhandler.h
ktar.h
ktemporaryfile.h
ktextbrowser.h
ktoggleaction.h
ktoolbar.h
ktoolinvocation.h
kurlcombobox.h
kurl.h
kuser.h
kvbox.h
kwidgetitemdelegate.h
kwindowsystem.h
kxmlguiclient.h
kxmlguifactory.h
kxmlguiwindow.h
kxyselector.h
kzip.h
kparts/browserextension.h
kparts/browserinterface.h
kparts/componentfactory.h
kparts/event.h
kparts/factory.h
kparts/mainwindow.h
kparts/part.h
kparts/partmanager.h
kparts/plugin.h
)

QT5_WRAP_CPP(kofake_HEADERS_MOC ${kofake_HEADERS})

kde4_add_library(kofake SHARED ${kofake_LIB_SRCS} ${kofake_HEADERS_MOC})

target_link_libraries(kofake Qt5::Widgets Qt5::Xml ${ZLIB_LIBRARIES})
target_link_libraries(kofake LINK_INTERFACE_LIBRARIES Qt5::Widgets Qt5::Xml)

set_target_properties(kofake PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS kofake ${INSTALL_TARGETS_DEFAULT_ARGS} )

