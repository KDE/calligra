// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2004-2006 Thorsten Zachmann <zachmann@kde.org>

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

#include "KPrBackground.h"
#include "KPrDocument.h"
#include <kapplication.h>
#include "KPrGradientCollection.h"
#include "KPrTextObject.h"
#include "KPrPage.h"
#include "KPrUtils.h"
#include "KPrBrush.h"

#include <qpainter.h>
#include <q3picture.h>
#include <qfileinfo.h>
//Added by qt3to4:
#include <QPixmap>
#include <QBuffer>

#include <KoRichText.h>
#include <KoTextObject.h>
// for getenv ()
#include <stdlib.h>
using namespace std;
#include <kdebug.h>
#include <kglobalsettings.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoGenStyles.h>
#include <KoXmlNS.h>

KPrBackGround::KPrBackGround( KPrPage *_page )
    // : footerHeight( 0 )
{
    backType = BT_COLOR;
    backView = BV_CENTER;
    backColor1 = Qt::white;
    backColor2 = Qt::white;
    bcType = BCT_PLAIN;
    unbalanced = false;
    xfactor = 100;
    yfactor = 100;

    gradientPixmap = 0L;
    m_page=_page;
}

void KPrBackGround::setBackPicture( const KoPicture& picture )
{
    if ( backType != BT_PICTURE )
        return;
    backPicture = pictureCollection()->insertPicture(picture);
}

void KPrBackGround::setBackPicture ( const KoPictureKey& key )
{
    if ( backType != BT_PICTURE )
        return;
    backPicture = pictureCollection()->findOrLoad(key.filename(), key.lastModified() );
}

void KPrBackGround::drawBackground( QPainter *_painter, const KoTextZoomHandler* zoomHandler,
                                   const QRect& _crect, bool _drawBorders )
{
    QRect pageRect = zoomHandler->zoomRectOld( m_page->getPageRect() );
    QRect crect = pageRect.intersect( _crect );
    if ( crect.isEmpty() )
        return;
    QSize ext = pageRect.size();
    drawBackground( _painter, ext, crect, _drawBorders );
}

void KPrBackGround::drawBackground( QPainter *_painter, const QSize& ext, const QRect& crect, bool _drawBorders )
{
    if ( m_page->displayBackground() )
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
}

void KPrBackGround::reload()
{
    if ( backType == BT_PICTURE || backType == BT_CLIPART )
        backPicture = pictureCollection()->insertPicture(backPicture);
    else
        backPicture.clear();
}


QDomElement KPrBackGround::save( QDomDocument &doc )
{
    QString tag = m_page->masterPage() == 0 ? "MASTERPAGE" : "PAGE";
    QDomElement page=doc.createElement( tag );
    QDomElement element;

    element=doc.createElement("BACKMASTER");
    element.setAttribute( "useMasterBackground",  static_cast<int>(m_page->useMasterBackground() ) );
    element.setAttribute( "displayMasterPageObject", static_cast<int>( m_page->displayObjectFromMasterPage() ));
    element.setAttribute( "displayBackground", static_cast<int>( m_page->displayBackground() ));
    page.appendChild(element);
    if ( !m_page->useMasterBackground() )
    {

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
            element = doc.createElement( "BACKPICTUREKEY" );

            backPicture.getKey().saveAttributes( element );
            page.appendChild( element );
        }
    }

    return page;
}


void KPrBackGround::saveOasisBackgroundPageStyle( KoGenStyle& stylepageauto, KoGenStyles& mainStyles )
{
    switch ( backType )
    {
        case BT_COLOR:
        {
            QBrush qbrush( backColor1 );
            KPrBrush brush( qbrush, backColor1, backColor2, bcType,
                            bcType == BCT_PLAIN ? FT_BRUSH : FT_GRADIENT,
                            unbalanced, xfactor, yfactor );
            brush.saveOasisFillStyle( stylepageauto, mainStyles );
        } break;
        case BT_BRUSH:
        case BT_CLIPART:
        case BT_PICTURE:
        {
            stylepageauto.addProperty("draw:fill", "bitmap" );
            stylepageauto.addProperty("draw:fill-image-name", saveOasisPictureStyle( mainStyles ) );
            QString repeat = "repeat";
            switch ( backView )
            {
                case BV_ZOOM:
                    repeat = "stretch";
                    break;
                case BV_CENTER:
                    repeat = "no-repeat";
                    break;
                case BV_TILED:
                    repeat = "repeat";
                    break;
            }
            stylepageauto.addProperty( "style:repeat", repeat );
        } break;
    }
}


QString KPrBackGround::saveOasisPictureStyle( KoGenStyles& mainStyles )
{
    //<draw:fill-image draw:name="Pattern" xlink:href="Pictures/100000000000005E0000005E43C87AF2.png" xlink:type="simple" xlink:show="embed" xlink:actuate="onLoad"/>
    KoGenStyle pictureStyle( KPrDocument::STYLE_PICTURE /*no family name*/ );
    pictureStyle.addAttribute( "xlink:show", "embed" );
    pictureStyle.addAttribute( "xlink:actuate", "onLoad" );
    pictureStyle.addAttribute( "xlink:type", "simple" );
    pictureStyle.addAttribute( "xlink:href", pictureCollection()->getOasisFileName(backPicture ) );
    return mainStyles.lookup( pictureStyle, "picture" );
}


void KPrBackGround::loadOasis(KoOasisContext & context )
{
    KoStyleStack& styleStack = context.styleStack();
    kDebug()<<"KPrBackGround::loadOasis()\n";
    styleStack.setTypeProperties( "drawing-page" );
    if ( styleStack.hasAttributeNS( KoXmlNS::draw, "fill" ) )
    {
        const QString fill = styleStack.attributeNS( KoXmlNS::draw, "fill" );
        kDebug(33001) <<"fill page type :" << fill << endl;
        if ( fill == "solid" || fill == "gradient" )
        {
            KPrBrush brush;
            brush.loadOasisFillStyle( context, "drawing-page" );
            if ( brush.getFillType() == FT_BRUSH )
            {
                setBackColor1( brush.getBrush().color() );
                setBackColorType( BCT_PLAIN );
            }
            else
            {
                setBackColor1( brush.getGColor1() );
                setBackColor2( brush.getGColor2() );
                setBackColorType( brush.getGType() );
                setBackUnbalanced( brush.getGUnbalanced() );
                setBackXFactor( brush.getGXFactor() );
                setBackYFactor( brush.getGYFactor() );
            }
            setBackType( BT_COLOR );
        }
        else if ( fill == "bitmap" )
        {
            QString style = styleStack.attributeNS( KoXmlNS::draw, "fill-image-name" );
            QDomElement* draw =context.oasisStyles().drawStyles()[style];

            const QString href( draw->attributeNS( KoXmlNS::xlink, "href", QString::null) );
            kDebug()<<" href: "<<href<<endl;
            if ( !href.isEmpty() )
            {
                QString strExtension;
                const int result=href.findRev(".");
                if (result>=0)
                {
                    strExtension=href.mid(result+1); // As we are using KoPicture, the extension should be without the dot.
                }
                QString filename(href);
                const KoPictureKey key(filename, QDateTime::currentDateTime(Qt::UTC));
                backPicture.setKey(key);

                KoStore* store = context.store();
                if ( store->open( filename ) )
                {
                    KoStoreDevice dev(store);
                    if ( !backPicture.load( &dev, strExtension ) )
                        kWarning() << "Cannot load picture: " << filename << " " << href << endl;
                    store->close();
                }
                pictureCollection()->insertPicture( key, backPicture );
            }

            if ( styleStack.hasAttributeNS( KoXmlNS::style, "repeat" ) )
            {
                QString repeat = styleStack.attributeNS( KoXmlNS::style, "repeat" );
                if ( repeat == "stretch" )
                    setBackView( BV_ZOOM );
                else if ( repeat == "no-repeat" )
                    setBackView( BV_CENTER );
                else
                    setBackView( BV_TILED );
            }
            else
                setBackView( BV_TILED );; // use tiled as default

            setBackType(BT_PICTURE);
        }
    }
}

void KPrBackGround::load( const QDomElement &element )
{
    //compatible with old file format kpresenter-1.4
    QDomElement e=element.namedItem("BACKMASTER").toElement();
    if ( !e.isNull() )
    {
        if ( e.hasAttribute( "useMasterBackground" ) )
        {
            m_page->setUseMasterBackground( (bool)e.attribute("useMasterBackground").toInt());
        }
        else //for kpresenter-1.4
            m_page->setUseMasterBackground( true );

        if(e.hasAttribute("displayMasterPageObject"))
        {
            m_page->setDisplayObjectFromMasterPage( (bool)e.attribute("displayMasterPageObject").toInt());
        }
        if(e.hasAttribute("displayBackground"))
        {
            m_page->setDisplayBackground( (bool)e.attribute("displayBackground").toInt());
        }
    }
    else
        m_page->setUseMasterBackground( false );

    e=element.namedItem("BACKTYPE").toElement();
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
            setBackColor1(QColor(Qt::red, Qt::green, Qt::blue));
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
            setBackColor2(QColor(Qt::red, Qt::green, Qt::blue));
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
                QByteArray rawData=_data.toUtf8(); // XPM is normally ASCII, therefore UTF-8
                rawData[rawData.size()-1]=char(10); // Replace the NULL character by a LINE FEED
                QBuffer buffer(&rawData);
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
}

void KPrBackGround::drawBackColor( QPainter *_painter, const QSize& ext, const QRect& crect )
{
    if ( (backType == BT_COLOR && bcType == BCT_PLAIN) || backColor1 == backColor2 ) //plain color
    {
        //kDebug(33001) << "KPrBackGround::drawBackColor (filling " << DEBUGRECT(crect) << ")" << endl;
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

void KPrBackGround::drawBackPix( QPainter *_painter, const QSize& ext, const QRect& /*crect*/ )
{
    /*kDebug(33001) << "KPrBackGround::drawBackPix ext=" << ext.width() << "," << ext.height() << endl;
      kDebug(33001) << "mode=" << (backView==BV_ZOOM?"ZOOM":backView==BV_TILED?"TILED":backView==BV_CENTER?"CENTER":"OTHER")
      << " crect=" << DEBUGRECT(crect) << endl;*/
    if ( !backPicture.isNull() )
    {
        // depend on page size and desktop size
        const QSize _origSize = backPicture.getOriginalSize();
        // NOTE: make all multiplications before any division
        double w = _origSize.width();
        w *= ext.width();
        QRect desk = KGlobalSettings::desktopGeometry(kapp->activeWindow());
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

void KPrBackGround::drawBorders( QPainter *_painter, const QSize& /*ext*/, const QRect& /*crect*/ )
{
    _painter->setPen( QApplication::palette().active().color( QColorGroup::Dark ) );
    _painter->setBrush( Qt::NoBrush );

     QRect rect = m_page->getZoomPageRect();

     _painter->drawRect( rect.x()-1,rect.y()-1,rect.width()+1,rect.height()+1 );
}

void KPrBackGround::generateGradient( const QSize& size )
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

void KPrBackGround::removeGradient()
{
    if ( gradientPixmap ) {
        gradientCollection()->removeRef( backColor1, backColor2, bcType, gradientPixmap->size(),
                                         unbalanced, xfactor, yfactor);
        gradientPixmap = 0;
    }
}

KoPictureCollection * KPrBackGround::pictureCollection() const
{
    return m_page->kPresenterDoc()->pictureCollection();
}

KPrGradientCollection * KPrBackGround::gradientCollection() const
{
    return m_page->kPresenterDoc()->gradientCollection();
}

void KPrBackGround::setBackGround( const struct Settings &settings )
{
    backType = settings.m_backType;
    backColor1 = settings.m_backColor1;
    backColor2 = settings.m_backColor2;
    bcType = settings.m_bcType;
    unbalanced = settings.m_unbalanced;
    xfactor = settings.m_xfactor;
    yfactor = settings.m_yfactor;
    removeGradient();
    setBackPicture( settings.m_backPictureKey );
    backView = settings.m_backView;

}

struct KPrBackGround::Settings KPrBackGround::getBackGround() const
{
    return Settings( backType, backColor1, backColor2,
                     bcType, unbalanced, xfactor, yfactor,
                     backPicture.getKey(), backView);
}
