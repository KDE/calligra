/*
 * This file is part of Calligra
 *
 * SPDX-FileCopyrightText: 2011-2012 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#include <QCoreApplication>

#include "CSTProcessRunner.h"

#include <QDebug>
#include <QTimer>

int main(int argc, char *argv[])
{
    if (argc < 4) {
        qDebug() << "usage" << argv[0] << "docDir resultDir concurrentProcesses [nopickup]";
        qDebug() << "   docDir               The directory where the test documents are";
        qDebug() << "   resultDir            The directory where the results should be saved.";
        qDebug() << "                        Will be created if it does not exist.";
        qDebug() << "   concurrentProcesses  Number of processes running at the same time to create results";
        qDebug() << "   nopickup             If you add 'nopickup' all results will be regenerated.";
        qDebug() << "                        The default is to continue the result generation.";

        exit(-1);
    }

    QCoreApplication app(argc, argv);

    QString docDir(argv[1]);
    QString resultDir(argv[2]);
    QString number(argv[3]);
    QString noPickup("pickup");
    if (argc >= 5) {
        noPickup = argv[4];
    }
    CSTProcessRunner runner(docDir, resultDir, number.toInt(), noPickup != "nopickup");
    QTimer::singleShot(0, &runner, &CSTProcessRunner::start);

    return app.exec();
}
