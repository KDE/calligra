#include <QtGui/QApplication>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QFile>

#include "qmlapplicationviewer.h"

#include "FileSystemModel.h"
#include "DocumentView.h"
#include "Settings.h"

#if defined(Q_WS_X11)
#include <X11/Xlib.h>
void preInitHacks() {
    // Nah, even if we are not using QtGui we still receive input
    // events which are processed what can lead to asserts/crashes
    // cause the main-app runs in another thread. Hack around by
    // allowing X11-stuff to run also outside of the Gui-thread.
    XInitThreads();
}
#else
void preInitHacks() {}
#endif

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    preInitHacks();

    QScopedPointer<QApplication> app(createApplication(argc, argv));

    QmlApplicationViewer viewer;
    QDeclarativeContext *ctxt = viewer.rootContext();

    qmlRegisterType<FileSystemModel>("FileSystemModel", 1, 0, "FileSystemModelItem");
    qmlRegisterType<DocumentView>("DocumentView", 1, 0, "DocumentViewItem");

    ctxt->setContextProperty("Settings", Settings::instance());

    QString url("qml/coffice/main.qml");

    // temporary hack for my dev-system :)
    QString hack = "/home/snoopy/src/calligra/coffice/coffice/qml/coffice/main.qml";
    if (QFile(hack).exists()) url = hack;

    viewer.setMainQmlFile(url);

    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.showExpanded();

    int result = app->exec();

    // Call that explicit since the dtor of Settings may not be called...
    Settings::instance()->saveChanges();

    return result;
}
