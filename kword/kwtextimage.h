/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef kwtextimage_h
#define kwtextimage_h

#include <kwimage.h>
#include <kwtextdocument.h>

/**
 * This class is used by "Insert Picture", i.e. having an image inline in a paragraph.
 */
class KWTextImage : public KWTextCustomItem
{
public:
    /**
     * Set filename to load a real file from the disk
     * Otherwise use setImage() - this is what's done on loading
     */
    KWTextImage( KWTextDocument *textdoc, const QString & filename );
    ~KWTextImage()
    {
        // Remove image from collection ?
    }

    Placement placement() const { return place; }
    void adjustToPainter( QPainter* );
    int widthHint() const { return width; }
    int minimumWidth() const { return width; }

    void setImage( const KWImage &image );
    KWImage image() const { return m_image; }

    void draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg );

private:
    Placement place;
    KWImage m_image;
};

#endif
