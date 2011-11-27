/* This file is part of the KDE project

   Copyright (C) 2011 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB. If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <QApplication>

#include <QDebug>

#include "CSTCompareView.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (argc < 4) {
        qDebug() << "usage" << argv[0] << "path1 path2 file [pagenumber] [--pdf pdffolder]";
        exit(-1);
    }

    QString pdfDir;
    int pdfPosition = app.arguments().indexOf("--pdf");
    if (pdfPosition > 0) {
        if (pdfPosition + 1 < app.arguments().size()) {
            pdfDir = app.arguments()[pdfPosition + 1];
        } else {
            qDebug() << "--pdf must be followed by the pdf directory";
            exit(-1);
        }
    }

    CSTCompareView cv;
    cv.open(argv[1], argv[2], pdfDir, argv[3]);
    cv.show();
    return app.exec();
}
