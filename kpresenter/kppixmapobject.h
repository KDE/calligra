/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef kppixmapobject_h
#define kppixmapobject_h

#include <qdatetime.h>

#include <kpobject.h>
#include <kpimage.h>
#include <koSize.h>
class KPGradient;
class QPixmap;

/******************************************************************/
/* Class: KPPixmapObject                                          */
/******************************************************************/

class KPPixmapObject : public KP2DObject
{
    friend class KPrCanvas;

public:
    KPPixmapObject( KPImageCollection *_imageCollection );
    KPPixmapObject( KPImageCollection *_imageCollection, const KPImageKey & key );
    virtual ~KPPixmapObject() {}

    KPPixmapObject &operator=( const KPPixmapObject & );

    virtual void setSize( double _width, double _height );
    virtual void setSize( const KoSize &_size )
    { setSize( _size.width(), _size.height() ); }
    virtual void resizeBy( double _dx, double _dy );

    // Only used as a default value in the filedialog, in changePicture
    // Don't use for anything else
    QString getFileName() const
    { return image.key().filename(); }

    KPImageKey getKey() const
    { return image.key(); }

    QSize originalSize() const
    { return image.originalSize(); }

    void setPixmap( const KPImageKey & key, const KoSize &_size = orig_size );

    void reload()
    { setPixmap( image.key(), ext ); }

    virtual ObjType getType() const
    { return OT_PICTURE; }
    virtual QString getTypeString() const
    { return i18n("Picture"); }

    virtual QDomDocumentFragment save( QDomDocument& doc, int offset );
    virtual int load(const QDomElement &element);

    virtual void draw( QPainter *_painter, KoZoomHandler*_zoomHandler, bool drawSelection );

protected:
    KPPixmapObject() {}

    KPImageCollection *imageCollection;
    KPImage image;
    KPGradient *gradient;
};

#endif
