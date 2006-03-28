/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>
   Copyright (C) 2002 Nicolas GOUTTE <goutte@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qimage.h>
#include <qpainter.h>
#include <qfile.h>
//Added by qt3to4:
#include <QPixmap>

#include <KoPicture.h>

#include "clipartcreator.h"
#include "koffice_export.h"

extern "C"
{
    KOFFICETOOLS_EXPORT ThumbCreator *new_creator()
    {
        return new ClipartCreator;
    }
}

bool ClipartCreator::create(const QString &path, int width, int height, QImage &img)
{
    QPixmap pixmap;
    KoPicture picture;
    if (picture.loadFromFile( path ))
    {
        pixmap = QPixmap( 200, 200 );
        pixmap.fill( Qt::white );

        QPainter p;
        p.begin( &pixmap );
        p.setBackgroundColor( Qt::white );

        picture.draw(p, 0, 0, pixmap.width(), pixmap.height());
        p.end();
        img = pixmap.convertToImage();
        return true;
    }
    else
        return false;
}

ThumbCreator::Flags ClipartCreator::flags() const
{
    return static_cast<Flags>(DrawFrame);
}
