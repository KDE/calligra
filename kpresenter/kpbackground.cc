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

#include <kpbackground.h>
#include <kpresenter_doc.h>
#include <qwmf.h>
#include <qapplication.h>
#include <kpgradientcollection.h>
#include <kptextobject.h>

#include <qpainter.h>
#include <qpicture.h>
#include <qfileinfo.h>

#include <qrichtext_p.h>
#include <kotextobject.h>
// for getenv ()
#include <stdlib.h>
using namespace std;
#include <kdebug.h>

/******************************************************************/
/* Class: KPBackGround                                            */
/******************************************************************/

/*================================================================*/
KPBackGround::KPBackGround( KPImageCollection *_imageCollection, KPGradientCollection *_gradientCollection,
                            KPClipartCollection *_clipartCollection, KPrPage *_page )
    : footerHeight( 0 )
{
    backType = BT_COLOR;
    backView = BV_CENTER;
    backColor1 = Qt::white;
    backColor2 = Qt::white;
    bcType = BCT_PLAIN;
    pageEffect = PEF_NONE;
    unbalanced = false;
    xfactor = 100;
    yfactor = 100;
    pageTimer = 1;
    soundEffect = false;
    soundFileName = QString::null;

    imageCollection = _imageCollection;
    gradientCollection = _gradientCollection;
    clipartCollection = _clipartCollection;
    gradient = 0L;
    m_page=_page;

    if( m_page)
      setBgSize( m_page->getZoomPageRect().size(),false);
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
    case BV_TILED: case BV_CENTER: pixSize = orig_size.toQSize();
        break;
    }

    backImage = imageCollection->findOrLoad( _filename, _lastModified );
    if ( pixSize == orig_size.toQSize() )
        pixSize = backImage.size();

    backImage = backImage.scale( pixSize );
}

/*================================================================*/
void KPBackGround::setBackClipart( const QString &_filename, QDateTime _lastModified )
{
    if ( backType != BT_CLIPART )
        return;

    //if ( picture )
    //    clipartCollection->removeRef( clipKey );

    backClipart = clipartCollection->findOrLoad( _filename, _lastModified );
}

/*================================================================*/
void KPBackGround::draw( QPainter *_painter, bool _drawBorders )
{
    _painter->save();
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
        if ( !backClipart.isNull() )
        {
            _painter->save();
            QRect br = backClipart.picture()->boundingRect();
            if ( br.width() && br.height() )
                _painter->scale( (double)ext.width() / (double)br.width(), (double)ext.height() / (double)br.height() );
            _painter->drawPicture( *backClipart.picture() );
            _painter->restore();
        }
    } break;
    }

    if ( _drawBorders )
        drawBorders( _painter );

    drawHeaderFooter( _painter );

    _painter->restore();
}

/*================================================================*/
void KPBackGround::restore()
{
    if ( backType == BT_PICTURE )
        setBackPixmap( backImage.key().filename(), backImage.key().lastModified() );
    if ( backType == BT_CLIPART )
	setBackClipart( backClipart.key().filename(), backClipart.key().lastModified() );
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
    QDomElement element;

    if (backType!=BT_COLOR) {
        element=doc.createElement("BACKTYPE");
        element.setAttribute("value", static_cast<int>( backType ));
        page.appendChild(element);
    }

    if (backView!=BV_CENTER) {
        element=doc.createElement("BACKVIEW");
        element.setAttribute("value", static_cast<int>( backView ));
        page.appendChild(element);
    }

    if (backColor1!=Qt::white) {
        element=doc.createElement("BACKCOLOR1");
        element.setAttribute("color", backColor1.name());
        page.appendChild(element);
    }

    if (backColor2!=Qt::white) {
        element=doc.createElement("BACKCOLOR2");
        element.setAttribute("color", backColor2.name());
        page.appendChild(element);
    }

    if (bcType!=BCT_PLAIN) {
        element=doc.createElement("BCTYPE");
        element.setAttribute("value", static_cast<int>( bcType ));
        page.appendChild(element);
    }

    if (xfactor!=100 || yfactor!=100 || unbalanced) {
        element=doc.createElement("BGRADIENT");
        element.setAttribute("unbalanced", static_cast<int>( unbalanced ));
        element.setAttribute("xfactor", xfactor);
        element.setAttribute("yfactor", yfactor);
        page.appendChild(element);
    }

    if ( !backImage.isNull() && backType == BT_PICTURE )
    {
        element = doc.createElement( "BACKPIXKEY" );
        backImage.key().saveAttributes( element );
        page.appendChild( element );
    }

    if ( !backClipart.isNull() && backType == BT_CLIPART )
    {
        element=doc.createElement( "BACKCLIPKEY" );
        backClipart.key().saveAttributes( element );
        page.appendChild( element );
    }

    if (pageEffect!=PEF_NONE) {
        element=doc.createElement("PGEFFECT");
        element.setAttribute("value", static_cast<int>( pageEffect ));
        page.appendChild(element);
    }

    if ( pageTimer != 1 ) {
        element = doc.createElement( "PGTIMER" );
        element.setAttribute( "timer", pageTimer );
        page.appendChild( element );
    }

    if ( soundEffect || !soundFileName.isEmpty() ) {
        element = doc.createElement( "PGSOUNDEFFECT" );
        element.setAttribute( "soundEffect", static_cast<int>(soundEffect) );
        element.setAttribute( "soundFileNmae", soundFileName );
        page.appendChild( element );
    }

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
        if(e.hasAttribute("color"))
            setBackColor1(QColor(e.attribute("color")));
        else
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
        if(e.hasAttribute("color"))
            setBackColor2(QColor(e.attribute("color")));
        else
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
        backImage = KPImage( key, QImage() ); // Image will be set by reload(), called by completeLoading()
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

            if ( ext == orig_size.toQSize() )
                ext = backImage.size();

            backImage = backImage.scale( ext );
        }
    }
    e=element.namedItem("BACKCLIPKEY").toElement();
    if(!e.isNull()) {
        KPClipartKey clipKey;
        clipKey.loadAttributes(e, clipartCollection->tmpDate(), clipartCollection->tmpTime());
        backClipart = KPClipart( clipKey, QPicture() ); // Picture will be set by reload(), called by completeLoading()
    }
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
            //KPClipartKey clipKey( _fileName, QDateTime( clipartCollection->tmpDate(),
            //                                            clipartCollection->tmpTime() ) );
            backClipart = clipartCollection->loadClipart( _fileName ); // load from disk !
        }
    }
    e=element.namedItem("PGTIMER").toElement();
    if(!e.isNull()) {
        int timer = 1;
        if(e.hasAttribute("timer"))
            timer=e.attribute("timer").toInt();
        setPageTimer(timer);
    }
    else
        setPageTimer(1);
    e=element.namedItem("PGSOUNDEFFECT").toElement();
    if(!e.isNull()) {
        if(e.hasAttribute("soundEffect"))
            soundEffect=static_cast<bool>(e.attribute("soundEffect").toInt());
        else
            soundEffect=false;

        if(e.hasAttribute("soundFileNmae"))
            soundFileName=e.attribute("soundFileNmae");
        else
            soundFileName=QString::null;
    }
    else {
        soundEffect=false;
        soundFileName=QString::null;
    }
}

/*================================================================*/
void KPBackGround::drawBackColor( QPainter *_painter )
{
    if ( getBackColorType() == BCT_PLAIN || getBackColor1() == getBackColor2() )
    {
        _painter->fillRect( 0, 0, ext.width(), ext.height(), QBrush( getBackColor1() ) );
    }
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
void KPBackGround::drawHeaderFooter( QPainter *_painter )
{
    if ( m_page->kPresenterDoc()->hasHeader() ) {
#if 0
        QSize s( doc->header()->textObject()->size() );

        QPoint pnt( doc->header()->textObject()->x(), doc->header()->textObject()->y() );

        // #### Reggie: not very efficient but ok for now
        if ( true /*doc->header()->textObject()->isModified()*/ ) {
            doc->header()->setSize( ext.width(), 10 );
            //qDebug( "resize h" );
        }
        doc->header()->setOrig( _offset.x(), _offset.y() );

        int pgnum = doc->backgroundList()->findRef( this );
        if ( pgnum == -1 )
            pgnum = 0;

        doc->header()->textObject()->setPageNum( ++pgnum );


        doc->header()->setSize( ext.width(), doc->header()->textObject()->document()->lastParag()->rect().bottom() + 1 );
#endif
        m_page->kPresenterDoc()->header()->draw( _painter, m_page->kPresenterDoc()->zoomHandler(), true );
#if 0
        if ( doc->header()->textObject()->isModified() )
            doc->header()->textObject()->resize( s );
        doc->header()->textObject()->move( pnt.x(), pnt.y() );
#endif
    }

    if ( m_page->kPresenterDoc()->hasFooter() ) {
#if 0
        QSize s( doc->footer()->textObject()->size() );
        QPoint pnt( doc->footer()->textObject()->x(), doc->footer()->textObject()->y() );

        // #### Reggie: not very efficient but ok for now
        if ( true ) { //doc->footer()->getKTextObject()->isModified() || footerHeight <= 0 )
            doc->footer()->setSize( ext.width(), 10 );

            footerHeight = doc->footer()->textObject()->document()->lastParag()->rect().bottom() + 1;
            doc->footer()->setSize( ext.width(), footerHeight );
        }

        doc->footer()->setOrig( _offset.x(), _offset.y() + ext.height() - footerHeight );

        int pgnum = doc->backgroundList()->findRef( this );
        if ( pgnum == -1 )
            pgnum = 0;
        doc->footer()->textObject()->setPageNum( ++pgnum );
#endif
        m_page->kPresenterDoc()->footer()->draw( _painter, m_page->kPresenterDoc()->zoomHandler(), true );
#if 0
        if ( doc->footer()->textObject()->isModified() )
            doc->footer()->textObject()->resize( s.width(), s.height() );
        doc->footer()->textObject()->move( pnt.x(), pnt.y() );
#endif
    }

}

/*================================================================*/
void KPBackGround::drawBorders( QPainter *_painter )
{
    _painter->setPen( QApplication::palette().active().color( QColorGroup::Dark ) );
    _painter->setBrush( Qt::NoBrush );
    _painter->drawRect( 0, 0, m_page->getZoomPageRect().width() + 1, m_page->getZoomPageRect().height() + 1 );
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
void KPBackGround::setBgSize( QSize _size, bool visible )
{
    if ( _size != getSize() )
    {
        removeGradient(); ext = _size; footerHeight = 0;
        if (visible)
            restore();
    }
}
