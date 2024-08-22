/*
 * Assists in creating thumbnails for Gemini's file views
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QProcessEnvironment>
#include <QTimer>

#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>

#include <calligra-version.h>

#include "ThumbnailHelperImpl.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main(int argc, char **argv)
{
#ifdef Q_OS_WIN
    SetErrorMode(SEM_NOGPFAULTERRORBOX);
#endif

    // needed as kdelibs4support linking plugins seem to inject activated drkonqi
    // TODO: fix it, that seems very wrong
    KCrash::setDrKonqiEnabled(false);

    QApplication app(argc, argv);

    KAboutData aboutData("calligrageminithumbnailer",
                         i18n("Calligra Gemini Thumbnailer"),
                         QStringLiteral(CALLIGRA_VERSION_STRING),
                         i18n("Calligra Gemini: Writing and Presenting at Home and on the Go"),
                         KAboutLicense::GPL,
                         i18n("(c) 1999-%1 The Calligra team and KO GmbH.\n", QString::number(CALLIGRA_YEAR)),
                         QString(),
                         QStringLiteral("https://www.calligra.org"),
                         QStringLiteral("submit@bugs.kde.org"));

    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);

    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("in"), i18n("Document to thumbnail"), QStringLiteral("local-url")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("out"), i18n("The full path for the thumbnail file"), QStringLiteral("local-url")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("width"), i18n("The width in pixels of the thumbnail"), QStringLiteral("pixels")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("height"), i18n("The height in pixels of the thumbnail"), QStringLiteral("pixels")));

    parser.process(app);
    aboutData.processCommandLine(&parser);

#ifdef Q_OS_WIN
    QDir appdir(app.applicationDirPath());
    appdir.cdUp();

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (!env.contains("KDESYCOCA")) {
        _putenv_s("KDESYCOCA", QString(appdir.absolutePath() + "/sycoca").toLocal8Bit());
    }
    if (!env.contains("XDG_DATA_DIRS")) {
        _putenv_s("XDG_DATA_DIRS", QString(appdir.absolutePath() + "/share").toLocal8Bit());
    }
    _putenv_s("PATH",
              QString(appdir.absolutePath() + "/bin" + ";" + appdir.absolutePath() + "/lib" + ";" + appdir.absolutePath() + "/lib" + "/kde4" + ";"
                      + appdir.absolutePath())
                  .toLocal8Bit());

    app.addLibraryPath(appdir.absolutePath());
    app.addLibraryPath(appdir.absolutePath() + "/bin");
    app.addLibraryPath(appdir.absolutePath() + "/lib");
    app.addLibraryPath(appdir.absolutePath() + "/lib/kde4");
#endif

    QString inFile = parser.value("in");
    QString outFile = parser.value("out");
    // Only create the thunbnail if:
    // 1) The infile exists and
    // 2) The outfile does /not/ exist
    if (!QFile::exists(inFile)) {
        qDebug() << "The document you are attempting to create a thumbnail of does not exist on disk:" << inFile;
    } else if (QFile::exists(outFile)) {
        qDebug() << "The thumbnail file you are asking to have used already exists on disk. We will refuse to overwrite it." << outFile;
    } else {
        ThumbnailHelperImpl helper;
        helper.convert(inFile, outFile, parser.value("width").toInt(), parser.value("height").toInt());
    }
    QTimer::singleShot(0, &app, &QApplication::quit);

    return app.exec();
}
