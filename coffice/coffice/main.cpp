#include <QtGui/QApplication>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QFile>
#include <QWindowsStyle>
#include <QDebug>

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

    // Disable desktop-aware settings like for system's standard
    // colors, fonts, etc. to not clash with whatever QWidgets
    // dsktop related things are configured. We are not using
    // QWidgets and trust in our and Qt's configurations.
    QApplication::setDesktopSettingsAware(false);

    // We are not using QWidget's but Qt may still load and initialize
    // a style. Let's hard-code to a buildin style to be sure Oxygen or
    // another style using kdelibs isn't picked up and potentially
    // clashes with symbols defined by our fake library.
    QApplication::setStyle(new QWindowsStyle);
}
#else
void preInitHacks() {}
#endif

#ifdef Q_OS_ANDROID
#include <jni.h>
extern "C" {
    // Used by QtActivity.java to pass in the file-uri that should be loaded on startup.
    JNIEXPORT void JNICALL Java_org_kde_necessitas_origo_QtActivity_openFileIntent(JNIEnv *jenv, jobject, jstring uri) {
        const char* file = jenv->GetStringUTFChars(uri, 0);
        Settings::instance()->setOpenFileRequested(QString::fromUtf8(file));
        jenv->ReleaseStringUTFChars(uri, file);
    }
}
#endif

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    preInitHacks();

    QScopedPointer<QApplication> app(createApplication(argc, argv));

    for(int i = 1; i < QCoreApplication::arguments().count(); ++i) {
        QString arg = QCoreApplication::arguments().at(i);
        if (QFile(arg).exists()) {
            Settings::instance()->setOpenFileRequested(arg);
            break;
        }
    }

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
