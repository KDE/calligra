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

#include "imagecollection.h"
#include "kwdoc.h"

#include <qfont.h>
#include <qcolor.h>

/******************************************************************/
/* Class: KWImageCollection                                       */
/******************************************************************/

/*================================================================*/
KWImageCollection::KWImageCollection( KWDocument *_doc )
    : images( 1999, true )
{
    images.setAutoDelete( true );
    doc = _doc;
}

/*================================================================*/
KWImageCollection::~KWImageCollection()
{
    images.clear();
}

/*================================================================*/
KWImage *KWImageCollection::getImage( KWImage &_image, QString &key )
{
    key = "";

    key = generateKey( _image );

    KWImage *image = findImage( key );
    if ( image )
    {
        image->incRef();
        return image;
    }
    else
        return insertImage( key, _image );
}

/*================================================================*/
KWImage *KWImageCollection::getImage( KWImage &_image, QString &key, QSize _imgSize )
{
    key = "";

    key = generateKey( _image, _imgSize );
    QString key2 = generateKey( _image );

    KWImage *image = findImage( key );
    if ( image )
    {
        image->incRef();
        return image;
    }
    else
    {
        image = findImage( key2 );
        if ( image )
        {
            KWImage img( *image );
            return insertImage( key2, img, _imgSize );
        }
        return insertImage( key, _image, _imgSize );
    }
}

/*================================================================*/
void KWImageCollection::removeImage( KWImage *_image )
{
    QString key = generateKey( *_image );

    images.remove( key );
}

/*================================================================*/
QString KWImageCollection::generateKey( KWImage &_image )
{
    QString key;

    // Key: filename-width-height
    // e.g. /home/reggie/pics/kde.gif-40-36
    key.sprintf( "%s--%d-%d", _image.getFilename().latin1(),
                 _image.width(), _image.height() );
    return key;
}

/*================================================================*/
QString KWImageCollection::generateKey( KWImage &_image, QSize _imgSize )
{
    QString key;

    // Key: filename-width-height
    // e.g. /home/reggie/pics/kde.gif-40-36
    key.sprintf( "%s--%d-%d", _image.getFilename().latin1(),
                 _imgSize.width(), _imgSize.height() );
    return key;
}

/*================================================================*/
KWImage *KWImageCollection::findImage( QString _key )
{
    return images.find( _key.latin1() );
}

/*================================================================*/
KWImage *KWImageCollection::insertImage( QString _key, KWImage &_image )
{
    KWImage *image = new KWImage( doc, _image );

    images.insert( _key.latin1(), image );
    image->incRef();

    return image;
}

/*================================================================*/
KWImage *KWImageCollection::insertImage( QString _key, KWImage &_image, QSize _imgSize )
{
    KWImage *image = new KWImage( doc, _image );
    if ( !image->isNull() && image->size() != _imgSize )
    {
        QImage __image = image->smoothScale( _imgSize.width(), _imgSize.height() );
        delete image;
        image = new KWImage( doc, __image, _image.getFilename() );
    }

    images.insert( _key.latin1(), image );
    image->incRef();

    return image;
}

/*================================================================*/
KWImage *KWImageCollection::getImage( const QString filename )
{
    QDictIterator<KWImage> it( images );
    for ( ; it.current(); ++it )
    {
        if ( it.currentKey().contains( filename ) )
        {
            QString fn = it.currentKey();
            int dashdash = fn.findRev( "--" );
            if ( filename == fn.left( dashdash ) )
                return it.current();
        }
    }

    KWImage img( doc, filename );
    QString key;
    return getImage( img, key );
}
