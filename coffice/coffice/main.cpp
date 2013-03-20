#include <QtGui/QApplication>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QFile>

#include "qmlapplicationviewer.h"

#include "FileSystemModel.h"
#include "DocumentView.h"
#include "Settings.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
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
