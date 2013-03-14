#include <QtGui/QApplication>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QFile>

#include "qmlapplicationviewer.h"

#include "DocumentView.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    QmlApplicationViewer viewer;
    QDeclarativeContext *ctxt = viewer.rootContext();

    qmlRegisterType<DocumentView>("DocumentView", 1, 0, "DocumentViewItem");
    //ctxt->setContextProperty("Settings", &settings);

    QString url("qml/coffice/main.qml");

    // temporary hack for my dev-system :)
    QString hack = "/home/snoopy/src/calligra/coffice/coffice/qml/coffice/main.qml";
    if (QFile(hack).exists()) url = hack;

    viewer.setMainQmlFile(url);

    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.showExpanded();

    return app->exec();
}
