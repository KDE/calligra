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

#ifndef imagecollection_h
#define imagecollection_h

#include "image.h"

#include <qdict.h>
#include <qstring.h>

#include <qsize.h>

class KWDocument;

/******************************************************************/
/* Class: KWImageCollection                                       */
/******************************************************************/

class KWImageCollection
{
public:
    KWImageCollection( KWDocument *_doc );
    ~KWImageCollection();

    KWImage *getImage( KWImage &_image, QString &key );
    KWImage *getImage( KWImage &_image, QString &key, QSize _imgSize );
    void removeImage( KWImage *_image );
    KWImage *insertImage( QString _key, KWImage &_image );

    KWImage *getImage( const QString filename );

    QString generateKey( KWImage *_image )
    { return generateKey( *_image ); }

    KWImage *findImage( QString _key );

    QDictIterator<KWImage> iterator()
    { return QDictIterator<KWImage>( images ); }

protected:
    QString generateKey( KWImage &_image );
    QString generateKey( KWImage &_image, QSize _imgSize );
    KWImage *insertImage( QString _key, KWImage &_image, QSize _imgSize );

    QDict<KWImage> images;
    KWDocument *doc;

};

#endif
