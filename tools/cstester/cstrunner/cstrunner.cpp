/*
 * This file is part of Calligra
 *
 * Copyright (C) 2011 Thorsten Zachmann <thorsten.zachmann@kde.org>
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
        qDebug() << "usage" << argv[0] << "docDir resultDir concurrentProcesses";
        exit(-1);
    }

    QCoreApplication app(argc, argv);

    QString docDir(argv[1]);
    QString resultDir(argv[2]);
    QString number(argv[3]);
    CSTProcessRunner runner(docDir, resultDir, number.toInt());
    QTimer::singleShot(0, &runner, SLOT(start()));

    return app.exec();
}
