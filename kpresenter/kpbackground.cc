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

#include <KPresenterBackgroundIface.h>

#include <kpbackground.h>
#include <kpresenter_doc.h>
#include <qwmf.h>
#include <kpgradientcollection.h>
#include <kptextobject.h>

#include <qpicture.h>
#include <qpainter.h>
#include <qfileinfo.h>

// for getenv ()
#include <stdlib.h>
using namespace std;

/******************************************************************/
/* Class: KPBackGround                                            */
/******************************************************************/

/*================================================================*/
KPBackGround::KPBackGround( KPImageCollection *_imageCollection, KPGradientCollection *_gradientCollection,
                            KPClipartCollection *_clipartCollection, KPresenterDoc *_doc )
    : footerHeight( 0 )
{
    dcop = 0;
    backType = BT_COLOR;
    backView = BV_CENTER;
    backColor1 = Qt::white;
    backColor2 = Qt::white;
    bcType = BCT_PLAIN;
    pageEffect = PEF_NONE;
    unbalanced = false;
    xfactor = 100;
    yfactor = 100;

    imageCollection = _imageCollection;
    gradientCollection = _gradientCollection;
    clipartCollection = _clipartCollection;
    gradient = 0L;
    picture = 0L;

    doc = _doc;
}

/*================================================================*/
void KPBackGround::setBackPixmap( const QString &_filename, QDateTime _lastModified )
{
    if ( backType != BT_PICTURE )
        return;

    QSize pixSize;
    switch ( backView )
    {
    case BV_ZOOM: pixSize = QSize( ext.width(), ext.height() );
        break;
    case BV_TILED: case BV_CENTER: pixSize = orig_size;
        break;
    }

    backImage = imageCollection->findOrLoad( _filename, _lastModified );
    if ( pixSize == orig_size )
        pixSize = backImage.size();

    backImage = backImage.scale( pixSize );
}

/*================================================================*/
void KPBackGround::setBackClipFilename( const QString &_filename, QDateTime _lastModified )
{
    if ( backType != BT_CLIPART )
        return;

    if ( !_lastModified.isValid() )
    {
        QFileInfo inf( _filename );
        _lastModified = inf.lastModified();
    }

    if ( picture )
        clipartCollection->removeRef( clipKey );

    clipKey = KPClipartCollection::Key(_filename, _lastModified );
    picture = clipartCollection->findClipart( clipKey );
}

/*================================================================*/
void KPBackGround::draw( QPainter *_painter, QPoint _offset, bool _drawBorders )
{
    _painter->save();
    _painter->translate( _offset.x(), _offset.y() );

    switch ( backType )
    {
    case BT_COLOR:
        drawBackColor( _painter );
        break;
    case BT_PICTURE:
        if ( backView == BV_CENTER ) drawBackColor( _painter );
        drawBackPix( _painter );
        break;
    case BT_CLIPART:
    {
        drawBackColor( _painter );
        _painter->save();
        // We have to use setViewport here, but it doesn't cumulate with previous transformations
        // (e.g. painter translation set up by kword when embedding kpresenter...)   :(
        _painter->setViewport( _offset.x(), _offset.y(), ext.width(), ext.height() );
        ////_painter->scale( 1.0 * ext.width() / r.width(), 1.0 * ext.height() / r.height() );
        drawBackClip( _painter );
        _painter->restore();
    } break;
    }

    if ( _drawBorders )
        drawBorders( _painter );

    drawHeaderFooter( _painter, _offset );

    _painter->restore();
}

/*================================================================*/
void KPBackGround::restore()
{
    if ( backType == BT_PICTURE )
        setBackPixmap( backImage.key().filename(), backImage.key().lastModified() );

    if ( backType == BT_CLIPART )
	setBackClipFilename( clipKey.filename, clipKey.lastModified );

    if ( backType != BT_PICTURE )
        backImage = KPImage();

    if ( backType == BT_COLOR || backType == BT_CLIPART ||
	 backType == BT_PICTURE && backView == BV_CENTER ) {
	if ( gradient ) {
	    gradientCollection->removeRef( backColor1, backColor2, bcType, ext, unbalanced, xfactor, yfactor );
	    gradient = 0;
	}
	gradient = gradientCollection->getGradient( backColor1, backColor2, bcType, ext, unbalanced, xfactor, yfactor );
    }

    if ( backType == BT_PICTURE && backView != BV_CENTER && gradient ) {
	gradientCollection->removeRef( backColor1, backColor2, bcType, ext, unbalanced, xfactor, yfactor );
	gradient = 0;
    }
}

/*================================================================*/
QDomElement KPBackGround::save( QDomDocument &doc )
{
    QDomElement page=doc.createElement("PAGE");
    QDomElement element=doc.createElement("BACKTYPE");
    element.setAttribute("value", static_cast<int>( backType ));
    page.appendChild(element);
    element=doc.createElement("BACKVIEW");
    element.setAttribute("value", static_cast<int>( backView ));
    page.appendChild(element);
    element=doc.createElement("BACKCOLOR1");
    element.setAttribute("red", backColor1.red());
    element.setAttribute("green", backColor1.green());
    element.setAttribute("blue", backColor1.blue());
    page.appendChild(element);
    element=doc.createElement("BACKCOLOR2");
    element.setAttribute("red", backColor2.red());
    element.setAttribute("green", backColor2.green());
    element.setAttribute("blue", backColor2.blue());
    page.appendChild(element);
    element=doc.createElement("BCTYPE");
    element.setAttribute("value", static_cast<int>( bcType ));
    page.appendChild(element);
    element=doc.createElement("BGRADIENT");
    element.setAttribute("unbalanced", static_cast<int>( unbalanced ));
    element.setAttribute("xfactor", xfactor);
    element.setAttribute("yfactor", yfactor);
    page.appendChild(element);

    if ( !backImage.isNull() && backType == BT_PICTURE )
    {
        QDomElement elem = doc.createElement( "BACKPIXKEY" );
        backImage.key().saveAttributes( elem );
        page.appendChild( elem );
    }

    if ( picture && backType == BT_CLIPART )
        page.appendChild(clipKey.saveXML(doc));

    element=doc.createElement("PGEFFECT");
    element.setAttribute("value", static_cast<int>( pageEffect ));
    page.appendChild(element);
    return page;
}

/*================================================================*/
void KPBackGround::load( const QDomElement &element )
{
    QDomElement e=element.namedItem("BACKTYPE").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        setBackType(static_cast<BackType>(tmp));
    }
    e=element.namedItem("BACKVIEW").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        setBackView(static_cast<BackView>(tmp));
    }
    e=element.namedItem("BACKCOLOR1").toElement();
    if(!e.isNull()) {
        int red=0, green=0, blue=0;
        if(e.hasAttribute("red"))
            red=e.attribute("red").toInt();
        if(e.hasAttribute("green"))
            green=e.attribute("green").toInt();
        if(e.hasAttribute("blue"))
            blue=e.attribute("blue").toInt();
        setBackColor1(QColor(red, green, blue));
    }
    e=element.namedItem("BACKCOLOR2").toElement();
    if(!e.isNull()) {
        int red=0, green=0, blue=0;
        if(e.hasAttribute("red"))
            red=e.attribute("red").toInt();
        if(e.hasAttribute("green"))
            green=e.attribute("green").toInt();
        if(e.hasAttribute("blue"))
            blue=e.attribute("blue").toInt();
        setBackColor2(QColor(red, green, blue));
    }
    e=element.namedItem("PGEFFECT").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        setPageEffect(static_cast<PageEffect>(tmp));
    }
    e=element.namedItem("BGRADIENT").toElement();
    if(!e.isNull()) {
        int xf=0, yf=0, unbalanced=0;
        if(e.hasAttribute("xfactor"))
            xf=e.attribute("xfactor").toInt();
        setBackXFactor(xf);
        if(e.hasAttribute("yfactor"))
            yf=e.attribute("yfactor").toInt();
        setBackYFactor(yf);
        if(e.hasAttribute("unbalanced"))
            unbalanced=e.attribute("unbalanced").toInt();
        setBackUnbalanced(static_cast<bool>(unbalanced));
    }
    e=element.namedItem("BCTYPE").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        setBackColorType(static_cast<BCType>(tmp));
    }
    e=element.namedItem("BACKPIXKEY").toElement();
    if(!e.isNull()) {
        KPImageKey key;
        key.loadAttributes(e, imageCollection->tmpDate(), imageCollection->tmpTime());
        backImage = KPImage( key, QImage() );
    }
    else {
        // try to find a BACKPIX tag if the BACKPIXKEY is not available...
        KPImageKey key;
        e=element.namedItem("BACKPIX").toElement();
        if(!e.isNull()) {
            bool openPic = true;
            QString _data;
            QString _fileName;
            if(e.hasAttribute("data"))
                _data=e.attribute("data");
            if ( _data.isEmpty() )
                openPic = true;
            else
                openPic = false;
            if(e.hasAttribute("filename"))
                _fileName=e.attribute("filename");
            if ( !_fileName.isEmpty() )
            {
                if ( int _envVarB = _fileName.find( '$' ) >= 0 )
                {
                    int _envVarE = _fileName.find( '/', _envVarB );
                    QString path = getenv( QFile::encodeName(_fileName.mid( _envVarB, _envVarE-_envVarB )) );
                    _fileName.replace( _envVarB-1, _envVarE-_envVarB+1, path );
                }
            }
            if ( openPic )
                // !! this loads it from the disk (unless it's in the image collection already)
                backImage = imageCollection->loadImage( _fileName );
            else
            {
                QDateTime dateTime( imageCollection->tmpDate(), imageCollection->tmpTime() );
                KPImageKey key( _fileName, dateTime );
                backImage = imageCollection->loadXPMImage( key, _data );
            }

            if ( ext == orig_size )
                ext = backImage.size();

            backImage = backImage.scale( ext );
        }
    }
    e=element.namedItem("BACKCLIPKEY").toElement();
    if(!e.isNull())
        clipKey.loadAttributes(e, clipartCollection->tmpDate(), clipartCollection->tmpTime());
    else {
        // try to find a BACKCLIP tag if the BACKCLIPKEY is not available...
        e=element.namedItem("BACKCLIP").toElement();
        if(!e.isNull()) {
            QString _fileName;
            if(e.hasAttribute("filename"))
                _fileName=e.attribute("filename");
            if ( !_fileName.isEmpty() )
            {
                if ( int _envVarB = _fileName.find( '$' ) >= 0 )
                {
                    int _envVarE = _fileName.find( '/', _envVarB );
                    QString path = getenv( QFile::encodeName(_fileName.mid( _envVarB, _envVarE-_envVarB )) );
                    _fileName.replace( _envVarB-1, _envVarE-_envVarB+1, path );
                }
            }
            clipKey.filename = _fileName;
            clipKey.lastModified.setDate( clipartCollection->tmpDate() );
            clipKey.lastModified.setTime( clipartCollection->tmpTime() );
        }
    }
}

/*================================================================*/
void KPBackGround::drawBackColor( QPainter *_painter )
{
    if ( getBackColorType() == BCT_PLAIN || getBackColor1() == getBackColor2() )
        _painter->fillRect( 0, 0, ext.width(), ext.height(), QBrush( getBackColor1() ) );
    else if ( gradient )
        _painter->drawPixmap( 0, 0, *gradient );
}

/*================================================================*/
void KPBackGround::drawBackPix( QPainter *_painter )
{
    if ( !backImage.isNull() )
    {
        switch ( backView )
        {
        case BV_ZOOM:
            _painter->drawPixmap( 0, 0, backImage.pixmap() );
            break;
        case BV_TILED:
            _painter->drawTiledPixmap( 0, 0, ext.width(), ext.height(), backImage.pixmap() );
            break;
        case BV_CENTER:
        {
            QPixmap *pix = new QPixmap( ext.width(), ext.height() );
            bool delPix = true;
            int _x = 0, _y = 0;

            QPixmap backPix = backImage.pixmap();

            if ( backPix.width() > pix->width() && backPix.height() > pix->height() )
                bitBlt( pix, 0, 0, &backPix, backPix.width() - pix->width(), backPix.height() - pix->height(),
                        pix->width(), pix->height() );
            else if ( backPix.width() > pix->width() )
            {
                bitBlt( pix, 0, 0, &backPix, backPix.width() - pix->width(), 0,
                        pix->width(), backPix.height() );
                _y = ( pix->height() - backPix.height() ) / 2;
            }
            else if ( backPix.height() > pix->height() )
            {
                bitBlt( pix, 0, 0, &backPix, 0, backPix.height() - pix->height(),
                        backPix.width(), pix->height() );
                _x = ( pix->width() - backPix.width() ) / 2;
            }
            else
            {
                _x = ( pix->width() - backPix.width() ) / 2;
                _y = ( pix->height() - backPix.height() ) / 2;
                delPix = false;
                delete pix;
                pix = &backPix;
            }

            if ( pix && !pix->isNull() )
                _painter->drawPixmap( _x, _y, *pix );
            if ( delPix ) delete pix;
        }
        break;
        }
    }
}

/*================================================================*/
void KPBackGround::drawHeaderFooter( QPainter *_painter, const QPoint &_offset )
{
    if ( doc->hasHeader() ) {

        QSize s( doc->header()->getKTextObject()->size() );
        QPoint pnt( doc->header()->getKTextObject()->x(), doc->header()->getKTextObject()->y() );

        // #### Reggie: not very efficient but ok for now
        if ( true /*doc->header()->getKTextObject()->isModified()*/ ) {
            doc->header()->setSize( ext.width(), 10 );
            //qDebug( "resize h" );
        }
        doc->header()->setOrig( _offset.x(), _offset.y() );

        int pgnum = doc->backgroundList()->findRef( this );
        if ( pgnum == -1 )
            pgnum = 0;
#if 0
        doc->header()->getKTextObject()->setPageNum( ++pgnum );
#endif

        doc->header()->setSize( ext.width(), doc->header()->getKTextObject()->document()->lastParag()->rect().bottom() + 1 );

        doc->header()->draw( _painter, 0, 0 );

        if ( doc->header()->getKTextObject()->isModified() )
            doc->header()->getKTextObject()->resize( s );
        doc->header()->getKTextObject()->move( pnt.x(), pnt.y() );
    }

    if ( doc->hasFooter() ) {
        QSize s( doc->footer()->getKTextObject()->size() );
        QPoint pnt( doc->footer()->getKTextObject()->x(), doc->footer()->getKTextObject()->y() );

        // #### Reggie: not very efficient but ok for now
        if ( true ) { //doc->footer()->getKTextObject()->isModified() || footerHeight <= 0 )
            doc->footer()->setSize( ext.width(), 10 );

            footerHeight = doc->footer()->getKTextObject()->document()->lastParag()->rect().bottom() + 1;
            doc->footer()->setSize( ext.width(), footerHeight );
        }

        doc->footer()->setOrig( _offset.x(), _offset.y() + ext.height() - footerHeight );

        int pgnum = doc->backgroundList()->findRef( this );
        if ( pgnum == -1 )
            pgnum = 0;
#if 0
        doc->footer()->getKTextObject()->setPageNum( ++pgnum );
#endif
        doc->footer()->draw( _painter, 0, 0 );

        if ( doc->footer()->getKTextObject()->isModified() )
            doc->footer()->getKTextObject()->resize( s.width(), s.height() );

        doc->footer()->getKTextObject()->move( pnt.x(), pnt.y() );
    }
}

/*================================================================*/
void KPBackGround::drawBackClip( QPainter *_painter )
{
    if ( picture )
        _painter->drawPicture( *picture );
}

/*================================================================*/
void KPBackGround::drawBorders( QPainter *_painter )
{
    QPen pen( Qt::red, 1 );
    QBrush brush( Qt::NoBrush );

    _painter->setPen( pen );
    _painter->setBrush( brush );
    _painter->drawRect( 0, 0, ext.width() + 1, ext.height() + 1 );
}

/*================================================================*/
void KPBackGround::removeGradient()
{
    if ( gradient ) {
        gradientCollection->removeRef( backColor1, backColor2, bcType, ext,
                                       unbalanced, xfactor, yfactor);
        gradient = 0;
    }
}

/*=============================================================*/
DCOPObject* KPBackGround::dcopObject()
{
    if ( !dcop )
        dcop = new KPresenterBackgroundIface( this );

    return dcop;
}

/*=============================================================*/
void KPBackGround::setBgSize( QSize _size, bool visible )
{
    if ( _size != getSize() )
    {
        removeGradient(); ext = _size; footerHeight = 0;
        if (visible)
            restore();
    }
}
