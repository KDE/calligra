/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Image Collection                                       */
/******************************************************************/

#include "imagecollection.h"
#include "kword_doc.h"

#include <qfont.h>
#include <qcolor.h>

/******************************************************************/
/* Class: KWImageCollection                                       */
/******************************************************************/

/*================================================================*/
KWImageCollection::KWImageCollection( KWordDocument *_doc )
    : images( 1999, true, true )
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
KWImage *KWImageCollection::getImage( KWImage &_image, QString &key, KSize _imgSize )
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
    key.sprintf( "%s--%d-%d", _image.getFilename().data(),
                 _image.width(), _image.height() );
    return key;
}

/*================================================================*/
QString KWImageCollection::generateKey( KWImage &_image, KSize _imgSize )
{
    QString key;

    // Key: filename-width-height
    // e.g. /home/reggie/pics/kde.gif-40-36
    key.sprintf( "%s--%d-%d", _image.getFilename().data(),
                 _imgSize.width(), _imgSize.height() );
    return key;
}

/*================================================================*/
KWImage *KWImageCollection::findImage( QString _key )
{
    return images.find( _key.data() );
}

/*================================================================*/
KWImage *KWImageCollection::insertImage( QString _key, KWImage &_image )
{
    KWImage *image = new KWImage( doc, _image );

    images.insert( _key.data(), image );
    image->incRef();

    return image;
}

/*================================================================*/
KWImage *KWImageCollection::insertImage( QString _key, KWImage &_image, KSize _imgSize )
{
    KWImage *image = new KWImage( doc, _image );
    if ( image->size() != _imgSize )
    {
        QImage __image = image->smoothScale( _imgSize.width(), _imgSize.height() );
        delete image;
        image = new KWImage( doc, __image, _image.getFilename() );
    }

    images.insert( _key.data(), image );
    image->incRef();

    return image;
}


