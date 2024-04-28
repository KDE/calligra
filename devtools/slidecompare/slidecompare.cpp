/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "combinedview.h"
#include <QApplication>
#include <QPixmap>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    //    QPixmapCache::setCacheLimit(100000);

    CombinedView cv;
    cv.resize(630, 410);
    cv.show();
    if (argc > 1) {
        cv.openFile(argv[1]);
    }
    return app.exec();
}
