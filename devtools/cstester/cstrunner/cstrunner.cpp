/*
 * This file is part of Calligra
 *
 * Copyright (C) 2011-2012 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <QCoreApplication>

#include "CSTProcessRunner.h"

#include <QTimer>
#include <QDebug>

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
    QTimer::singleShot(0, &runner, SLOT(start()));

    return app.exec();
}
