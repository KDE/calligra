/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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
#include <QSplashScreen>
#include <QDebug>

#include <KIconLoader>
#include <KLocalizedString>
#include <KAboutData>

#include "MainWindow.h"

//#include "sketch/SketchInputContext.h"

#include <calligraversion.h>

int main( int argc, char** argv )
{
#if defined HAVE_X11
    QApplication::setAttribute(Qt::AA_X11InitThreads);
#endif

    QApplication app(argc, argv);

    KAboutData aboutData(QStringLiteral("calligragemini"),
                         i18n("Calligra Gemini"),
                         QStringLiteral(CALLIGRA_VERSION_STRING),
                         i18n("Calligra Gemini: Writing and Presenting at Home and on the Go"),
                         KAboutLicense::GPL,
                         i18n("(c) 1999-%1 The Calligra team and KO GmbH.\n", QStringLiteral(CALLIGRA_YEAR)),
                         QString(),
                         QStringLiteral("https://www.calligra.org"),
                         QStringLiteral("submit@bugs.kde.org"));

    app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings, true);
    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);

    parser.addPositionalArgument(QStringLiteral("[file(s)]"), i18n("Document to open"));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("vkb"), i18n("Use the virtual keyboard")));

    parser.process(app);
    aboutData.processCommandLine(&parser);

    QStringList fileNames;
    foreach(const QString &fileName, parser.positionalArguments()) {
        if (QFile::exists(fileName)) {
            fileNames << fileName;
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

    KIconLoader::global()->addAppDir("calligra");
    KIconLoader::global()->addAppDir("calligragemini");
    KIconLoader::global()->addAppDir("calligrawords");
    KIconLoader::global()->addAppDir("calligrastage");
    KIconLoader::global()->addAppDir("calligrasheets");

    app.processEvents();

    MainWindow window(fileNames);

    if (parser.isSet("vkb")) {
//        app.setInputContext(new SketchInputContext(&app));
    }

#ifdef Q_OS_WIN
    window.showMaximized();
#else
    window.show();
#endif

    return app.exec();
}
