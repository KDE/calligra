/* This file is part of the KDE libraries
    Copyright (C) 2004 Antonio Larrosa <larrosa@kde.org

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "kpixmapregionselectordialog.h"
#include <QPixmap>
#include <QImage>
#include <QFile>
#include <QApplication>
#include <iostream>

int main(int argc, char**argv)
{
    QApplication app(argc, argv);

    if (argc <= 1) {
        std::cerr << "Usage: kpixmapregionselectordialogtest <imageFile>" << std::endl;
        return 1;
    }

    QPixmap pix(QFile::decodeName(argv[1]));
    QImage image = KPixmapRegionSelectorDialog::getSelectedImage(pix, 100, 100);
    image.save(QStringLiteral("output.png"), "PNG");

    return 0;
}
