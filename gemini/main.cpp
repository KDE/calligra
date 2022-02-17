/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <QApplication>
#include <QCommandLineParser>
#include <QFontDatabase>
#include <QFile>
#include <QStringList>
#include <QString>
#include <QProcessEnvironment>
#include <QDir>
#include <QMessageBox>
#include <QDebug>
#include <QQuickWindow>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QApplication>

#include <KLocalizedString>
#include <KAboutData>

#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoMainWindow.h>
#include <KoGlobal.h>
#include <KoDocumentInfo.h>
#include <KoView.h>
#include <KoPart.h>
#include <KoDocumentEntry.h>
#include <KoFilterManager.h>
#include <part/KWFactory.h>
#include <stage/part/KPrDocument.h>
#include <stage/part/KPrFactory.h>
#include <stage/part/KPrViewModePresentation.h>
#include <KoAbstractGradient.h>
#include <KoZoomController.h>
#include <KoFileDialog.h>
#include <KoDialog.h>
#include <KoIcon.h>

#include "PropertyContainer.h"
#include "RecentFileManager.h"
#include "DocumentManager.h"
#include "Settings.h"
#include "DocumentListModel.h"
#include "Constants.h"
#include "SimpleTouchArea.h"
#include "ToolManager.h"
#include "ParagraphStylesModel.h"
#include "KeyboardModel.h"
#include "ScribbleArea.h"
#include "RecentImageImageProvider.h"
#include "RecentFilesModel.h"
#include "TemplatesModel.h"
#include "CloudAccountsModel.h"

//#include "sketch/SketchInputContext.h"

#include <calligraversion.h>

int main( int argc, char** argv )
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

#if defined HAVE_X11
    QApplication::setAttribute(Qt::AA_X11InitThreads);
#endif
    QIcon::setFallbackThemeName("breeze");
    QApplication app(argc, argv);
    // Default to org.kde.desktop style unless the user forces another style
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

#ifdef Q_OS_WINDOWS
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }

    QApplication::setStyle(QStringLiteral("breeze"));
    auto font = app.font();
    font.setPointSize(10);
    app.setFont(font);
#endif

    KAboutData aboutData(QStringLiteral("calligragemini"),
                         i18n("Calligra Gemini"),
                         QStringLiteral(CALLIGRA_VERSION_STRING),
                         i18n("Calligra Gemini: Writing and Presenting at Home and on the Go"),
                         KAboutLicense::GPL,
                         i18n("(c) 1999-%1 The Calligra team and KO GmbH.\n", QStringLiteral(CALLIGRA_YEAR)),
                         QString(),
                         QStringLiteral("https://www.calligra.org"),
                         QStringLiteral("submit@bugs.kde.org"));

    aboutData.addAuthor(i18n("Carl Schwan"), QString(), QStringLiteral("carl@carlschwan.eu"));
    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);

    parser.addPositionalArgument(QStringLiteral("[file(s)]"), i18n("Document to open"));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("vkb"), i18n("Use the virtual keyboard")));

    parser.process(app);
    aboutData.processCommandLine(&parser);

    for(const QString &fileName : parser.positionalArguments()) {
        if (QFile::exists(fileName)) {
            DocumentManager::instance()->recentFileManager()->addRecent(QDir::current().absoluteFilePath(fileName));
        }
    }

#ifdef Q_OS_WIN
    QDir appdir(app.applicationDirPath());
    appdir.cdUp();

    QString envStringSet;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (!env.contains("XDG_DATA_DIRS")) {
        _putenv_s("XDG_DATA_DIRS", QString(appdir.absolutePath() + "/bin/data").toLocal8Bit());
        envStringSet.append("XDG_DATA_DIRS ");
    }
    _putenv_s("PATH", QString(appdir.absolutePath() + "/bin" + ";"
              + appdir.absolutePath() + "/lib" + ";"
              + appdir.absolutePath() + "/lib"  +  "/kde4" + ";"
              + appdir.absolutePath()).toLocal8Bit());

    if(envStringSet.length() > 0) {
        qDebug() << envStringSet << "were set from main, restarting application in new environment!";
        // Pass all the arguments along, but don't include the application name...
        QStringList allArguments;
        for(int i = 0; i < argc; i++) {
            allArguments << argv[i];
        }
        QProcess::startDetached(app.applicationFilePath(), allArguments.mid(1));
        exit(0);
    }

    app.addLibraryPath(appdir.absolutePath());
    app.addLibraryPath(appdir.absolutePath() + "/bin");
    app.addLibraryPath(appdir.absolutePath() + "/lib");
    app.addLibraryPath(appdir.absolutePath() + "/lib/kde4");
#endif


    qmlRegisterUncreatableType<PropertyContainer>("org.calligra", 1, 0, "PropertyContainer", "Contains properties and naively extends QML to support dynamic properties");
    qmlRegisterType<DocumentListModel>("org.calligra", 1, 0, "DocumentListModel");
    qmlRegisterType<SimpleTouchArea>("org.calligra", 1, 0, "SimpleTouchArea");
    qmlRegisterType<ToolManager>("org.calligra", 1, 0, "ToolManager");
    qmlRegisterType<ParagraphStylesModel>("org.calligra", 1, 0, "ParagraphStylesModel");
    qmlRegisterType<KeyboardModel>("org.calligra", 1, 0, "KeyboardModel");
    qmlRegisterType<ScribbleArea>("org.calligra", 1, 0, "ScribbleArea");
    qmlRegisterType<RecentFilesModel>("org.calligra", 1, 0, "RecentFilesModel");
    qmlRegisterType<TemplatesModel>("org.calligra", 1, 0, "TemplatesModel");
    qmlRegisterType<CloudAccountsModel>("org.calligra", 1, 0, "CloudAccountsModel");
    qmlRegisterType<KPrViewModePresentation>();
    qRegisterMetaType<QAction*>();
    qmlRegisterSingletonInstance("org.calligra", 1, 0, "DocumentManager", DocumentManager::instance());

    Settings settings;
    qmlRegisterSingletonInstance("org.calligra", 1, 0, "Settings", &settings);
    qmlRegisterSingletonInstance("org.calligra", 1, 0, "RecentFileManager", DocumentManager::instance()->recentFileManager());

    qmlRegisterSingletonType("org.calligra", 1, 0, "MimeType", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QJSValue {
        Q_UNUSED(engine)
    
        QJSValue mimeTypes = scriptEngine->newObject();
        mimeTypes.setProperty("words", QString(WORDS_MIME_TYPE));
        mimeTypes.setProperty("stage", QString(STAGE_MIME_TYPE));
        return mimeTypes;
    });

    app.processEvents();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    KLocalizedString::setApplicationDomain("neochat");
    engine.addImageProvider(QLatin1String("recentimage"), new RecentImageImageProvider);
    QObject::connect(&engine, &QQmlApplicationEngine::quit, &app, &QCoreApplication::quit);
    engine.load(QUrl(QStringLiteral("qrc:/qml/WelcomePage.qml")));

    return app.exec();
}
