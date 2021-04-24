/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
