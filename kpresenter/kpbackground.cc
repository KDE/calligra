// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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

#include "kpbackground.h"
#include "kpresenter_doc.h"
#include <kapplication.h>
#include "kpgradientcollection.h"
#include "kptextobject.h"
#include "kprpage.h"

#include <qpainter.h>
#include <qpicture.h>
#include <qfileinfo.h>

#include <korichtext.h>
#include <kotextobject.h>
// for getenv ()
#include <stdlib.h>
using namespace std;
#include <kdebug.h>
#include <kglobalsettings.h>

KPBackGround::KPBackGround( KPrPage *_page )
    // : footerHeight( 0 )
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

    gradientPixmap = 0L;
    m_page=_page;
}

void KPBackGround::setBackPicture( const KoPicture& picture )
{
    if ( backType != BT_PICTURE )
        return;
    backPicture = pictureCollection()->insertPicture(picture);
}

void KPBackGround::setBackPicture ( const KoPictureKey& key )
{
    if ( backType != BT_PICTURE )
        return;
    backPicture = pictureCollection()->findOrLoad(key.filename(), key.lastModified() );
}

void KPBackGround::draw( QPainter *_painter, const KoZoomHandler* zoomHandler,
                         const QRect& _crect, bool _drawBorders )
{
    QRect pageRect = zoomHandler->zoomRect( m_page->getPageRect() );
    QRect crect = pageRect.intersect( _crect );
    if ( crect.isEmpty() )
        return;
    QSize ext = pageRect.size();
    draw( _painter, ext, crect, _drawBorders );
}

void KPBackGround::draw( QPainter *_painter, const QSize& ext, const QRect& crect, bool _drawBorders )
{
    _painter->save();
    switch ( backType )
    {
    case BT_COLOR:
        drawBackColor( _painter, ext, crect );
        break;
    case BT_CLIPART:
    case BT_PICTURE:
    {
        if ( backView == BV_CENTER )
            drawBackColor( _painter, ext, crect );
        drawBackPix( _painter, ext, crect );
        break;
    }
    default:
        break;
    }

    if ( _drawBorders )
        drawBorders( _painter, ext, crect );

    _painter->restore();
}

void KPBackGround::reload()
{
    if ( backType == BT_PICTURE || backType == BT_CLIPART )
        backPicture = pictureCollection()->insertPicture(backPicture);
    else
        backPicture.clear();
}

QDomElement KPBackGround::save( QDomDocument &doc, const bool saveAsKOffice1Dot1 )
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

    if ( !backPicture.isNull() && ( ( backType == BT_PICTURE ) || ( backType == BT_CLIPART ) ) )
    {
        if (saveAsKOffice1Dot1) // KOffice 1.1
        {
            if ( backPicture.isClipartAsKOffice1Dot1() )
                element=doc.createElement( "BACKCLIPKEY" );
            else
                element = doc.createElement( "BACKPIXKEY" );
        }
        else
            element = doc.createElement( "BACKPICTUREKEY" );

        backPicture.getKey().saveAttributes( element );
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
        element.setAttribute( "soundFileName", soundFileName );
        page.appendChild( element );
    }

    return page;
}

void KPBackGround::loadOasis( const KoStyleStack &styleStack )
{
    // background
    kdDebug()<<"void KPBackGround::loadOasis( const KoStyleStack &styleStack )**********************************\n";
    if ( styleStack.hasAttribute( "draw:fill" ) )
    {
        kdDebug()<<" fill page \n";
        const QString fill = styleStack.attribute( "draw:fill" );
        kdDebug()<<" type :"<<fill<<endl;
        if ( fill == "solid" )
        {
            kdDebug()<<"solid \n";
            setBackColor1(QColor(styleStack.attribute( "draw:fill-color" ) ) );
            setBackColorType(BCT_PLAIN);
            setBackType(BT_COLOR);
        }
    }
}

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
    e=element.namedItem("BACKPICTUREKEY").toElement();
    if(!e.isNull()) {
        KoPictureKey key;
        key.loadAttributes( e );
        backPicture.clear();
        backPicture.setKey( key );
        // Image will be set by reload(), called by completeLoading()
    }
    e=element.namedItem("BACKPIXKEY").toElement();
    if(!e.isNull()) {
        KoPictureKey key;
        key.loadAttributes( e );
        backPicture.clear();
        backPicture.setKey( key );
        // Image will be set by reload(), called by completeLoading()
    }
    else {
        // try to find a BACKPIX tag if the BACKPIXKEY is not available...
        KoPictureKey key;
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
                backPicture = pictureCollection()->loadPicture( _fileName );
            else
            {
                KoPictureKey key( _fileName );
                backPicture.clear();
                backPicture.setKey(key);
                QByteArray rawData=_data.utf8(); // XPM is normally ASCII, therefore UTF-8
                rawData[rawData.size()-1]=char(10); // Replace the NULL character by a LINE FEED
                QBuffer buffer(rawData);
                backPicture.loadXpm(&buffer);
            }

#if 0
            if ( ext == orig_size.toQSize() )
                ext = backPicture.size();

            backPicture = backPicture.scale( ext );
#endif
        }
    }
    e=element.namedItem("BACKCLIPKEY").toElement();
    if(!e.isNull()) {
        KoPictureKey clipKey;
        clipKey.loadAttributes( e );
        backPicture.clear();
        backPicture.setKey(clipKey);
        // Picture will be set by reload(), called by completeLoading()
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
            //KPClipartKey clipKey( _fileName, QDateTime( pictureCollection()->tmpDate(),
            //                                            pictureCollection()->tmpTime() ) );
            backPicture = pictureCollection()->loadPicture( _fileName ); // load from disk !
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

        if(e.hasAttribute("soundFileNmae")) // old typo
            soundFileName=e.attribute("soundFileNmae");
        else
            soundFileName=e.attribute("soundFileName");
    }
    else {
        soundFileName=QString::null;
    }
}

void KPBackGround::drawBackColor( QPainter *_painter, const QSize& ext, const QRect& crect )
{
    if ( (backType == BT_COLOR && bcType == BCT_PLAIN) || backColor1 == backColor2 ) //plain color
    {
        //kdDebug(33001) << "KPBackGround::drawBackColor (filling " << DEBUGRECT(crect) << ")" << endl;
        _painter->fillRect( crect, QBrush( getBackColor1() ) );
    }
    else if (backType == BT_COLOR && bcType != BCT_PLAIN) { //gradient
        if ( !gradientPixmap || gradientPixmap->size() != ext )
            generateGradient( ext );
        _painter->drawPixmap( crect.topLeft(), *gradientPixmap, crect );
    }
    else /*if ( backType == BT_CLIPART || backType == BT_PICTURE )*/ //no gradient or bg color
        _painter->fillRect( crect, QBrush( Qt::white ) );
    return;
}

void KPBackGround::drawBackPix( QPainter *_painter, const QSize& ext, const QRect& /*crect*/ )
{
    /*kdDebug(33001) << "KPBackGround::drawBackPix ext=" << ext.width() << "," << ext.height() << endl;
      kdDebug(33001) << "mode=" << (backView==BV_ZOOM?"ZOOM":backView==BV_TILED?"TILED":backView==BV_CENTER?"CENTER":"OTHER")
      << " crect=" << DEBUGRECT(crect) << endl;*/
    if ( !backPicture.isNull() )
    {
        // depend on page size and desktop size
        const QSize _origSize = backPicture.getOriginalSize();
        // NOTE: make all multiplications before any division
        double w = _origSize.width();
        w *= ext.width();
#if KDE_IS_VERSION(3,1,90)
        QRect desk = KGlobalSettings::desktopGeometry(kapp->activeWindow());
#else
        QRect desk = QApplication::desktop()->screenGeometry();
#endif
        w /= desk.width();
        double h = _origSize.height();
        h *= ext.height();
        h /= desk.height();
        const QSize _pixSize = QSize( (int)w, (int)h );
        QPixmap backPix;

        switch ( backView )
        {
        case BV_ZOOM:
            backPix=backPicture.generatePixmap( ext, true );
            _painter->drawPixmap( QRect( 0, 0, ext.width(), ext.height() ), backPix );
            break;
        case BV_TILED:
            backPix=backPicture.generatePixmap( _pixSize, true );
            _painter->drawTiledPixmap( 0, 0, ext.width(), ext.height(), backPix );
            break;
        case BV_CENTER:
        {
            backPix=backPicture.generatePixmap( _pixSize, true );

            QPixmap *pix = new QPixmap( ext.width(), ext.height() );
            bool delPix = true;
            int _x = 0, _y = 0;

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

void KPBackGround::drawBorders( QPainter *_painter, const QSize& ext, const QRect& /*crect*/ )
{
    _painter->setPen( QApplication::palette().active().color( QColorGroup::Dark ) );
    _painter->setBrush( Qt::NoBrush );
    _painter->drawRect( 0, 0, ext.width() + 1, ext.height() + 1 );
}

void KPBackGround::generateGradient( const QSize& size )
{
    if ( backType == BT_COLOR || backType == BT_CLIPART ||
         backType == BT_PICTURE && backView == BV_CENTER ) {
        removeGradient();
        gradientPixmap = &gradientCollection()->getGradient( backColor1, backColor2, bcType, size,
                                                             unbalanced, xfactor, yfactor );
    }

    // Avoid keeping an unused gradient around
    if ( (backType == BT_PICTURE || backType==BT_CLIPART) && gradientPixmap )
        removeGradient();
}

void KPBackGround::removeGradient()
{
    if ( gradientPixmap ) {
        gradientCollection()->removeRef( backColor1, backColor2, bcType, gradientPixmap->size(),
                                         unbalanced, xfactor, yfactor);
        gradientPixmap = 0;
    }
}

KoPictureCollection * KPBackGround::pictureCollection() const
{
    return m_page->kPresenterDoc()->pictureCollection();
}

KPGradientCollection * KPBackGround::gradientCollection() const
{
    return m_page->kPresenterDoc()->gradientCollection();
}
