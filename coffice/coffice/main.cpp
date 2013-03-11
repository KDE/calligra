#include <QtGui/QApplication>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>

#include "qmlapplicationviewer.h"

#include "DocumentView.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));

    QmlApplicationViewer viewer;
    QDeclarativeContext *ctxt = viewer.rootContext();

    qmlRegisterType<DocumentView>("DocumentView", 1, 0, "DocumentViewItem");
    //ctxt->setContextProperty("Manager", &donkeyManager);

    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/coffice/main.qml"));
    viewer.showExpanded();

    return app->exec();
}
