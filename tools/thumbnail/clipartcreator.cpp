/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qimage.h>
#include <qpainter.h>
#include <qpicture.h>
#include <koClipartCollection.h>

#include "clipartcreator.h"

extern "C"
{
    ThumbCreator *new_creator()
    {
        return new ClipartCreator;
    }
};

bool ClipartCreator::create(const QString &path, int width, int height, QImage &img)
{
    QPixmap pixmap;
    QPicture pic;
    if ( KoClipartCollection::loadFromFile( path, &pic ) )
    {
        pixmap = QPixmap( width, height );
        QPainter p;

        p.begin( &pixmap );
        p.setBackgroundColor( Qt::white );
        pixmap.fill( Qt::white );

        QRect br = pic.boundingRect();
        if ( br.width() && br.height() )
            p.scale( (double)pixmap.width() / (double)br.width(), (double)pixmap.height() / (double)br.height() );
        p.drawPicture( pic );
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
