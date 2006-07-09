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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef kwtextimage_h
#define kwtextimage_h

#include <KoPictureKey.h>
#include <KoPicture.h>

#include <KWTextDocument.h>

/**
 * This class is used by "Insert Picture", i.e. having an image inline in a paragraph.
 * It is going to be removed from KWord (inline picture frames are so much better),
 * but we might need it in KPresenter !
 */
class KWTextImage : public KoTextCustomItem
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

    virtual Placement placement() const { return place; }
    virtual void resize();
    virtual int widthHint() const { return width; }
    virtual int minimumWidth() const { return width; }

    void setImage( const KoPictureCollection & collection );

    KoPicture image() const { return m_image; }

    virtual void drawCustomItem( QPainter* p, int x, int y, int wpix, int hpix, int ascentpix, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected, int offset, bool drawingShadow );

    // Save to XML
    virtual void save( QDomElement & formatElem );
    virtual void saveOasis( KoXmlWriter&, KoSavingContext& ) const;
    virtual int typeId() const { return 2; }

    void load( QDomElement & formatElem );

    /**
     * Get the key of the picture
     */
    KoPictureKey getKey( void ) const;
private:
    Placement place;
    KoPicture m_image;
    QSize m_size; // Size of image
};

#endif
