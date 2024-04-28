/*
 * This file is part of Calligra
 *
 * SPDX-FileCopyrightText: Dag Andersen <dag.andersen@kmail.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#include "Autocorrection.h"
#include <ACConfig.h>

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QString loDefault = QStringLiteral("/usr/lib/libreoffice/share/autocorr");
    QString calligraDefault = QStringLiteral(AUTOCORRECTIONDATA);

    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("updateautocorrection");
    QCommandLineParser parser;
    parser.setApplicationDescription("Updates calligra autocorrection files with data from libreoffice");
    parser.addHelpOption();
    QCommandLineOption loOption(QStringList() << QStringLiteral("l") << QStringLiteral("libreoffice"),
                                QStringLiteral("<directory> Where to find LibreOffice files.\nDefault: %1").arg(loDefault),
                                QStringLiteral("directory"));
    parser.addOption(loOption);
    QCommandLineOption calligraOption(QStringList() << QStringLiteral("c") << QStringLiteral("calligra"),
                                      QStringLiteral("<directory> Where to find and store Calligra files.\nDefault: %1").arg(calligraDefault),
                                      QStringLiteral("directory"));
    parser.addOption(calligraOption);
    parser.process(app);

    QString libreoffice = parser.value(loOption);
    if (libreoffice.isEmpty()) {
        libreoffice = loDefault;
    }
    QString calligra = parser.value(calligraOption);
    if (calligra.isEmpty()) {
        calligra = calligraDefault;
    }
    qInfo() << '\n';
    qInfo() << "======" << app.applicationName();
    qInfo() << "LibreOffice:" << libreoffice;
    qInfo() << "Calligra:" << calligra;
    qInfo() << "======\n";

    AutoCorrection c;
    c.updateAutoCorrections(libreoffice, calligra);

    qInfo() << '\n';
    qInfo() << "====== Report:";
    qInfo() << "New languages:" << c.newLanguages.count();
    if (!c.newLanguages.isEmpty()) {
        qInfo() << '\t' << c.newLanguages;
    }
    qInfo() << "Modified languages:" << c.modifiedLanguages.count();
    if (!c.modifiedLanguages.isEmpty()) {
        qInfo() << '\t' << c.modifiedLanguages;
    }
    qInfo() << "======\n";
}
