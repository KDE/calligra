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
#include "kpresenter_utils.h"

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
#include <koStore.h>
#include <koStoreDevice.h>
#include <koGenStyles.h>

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

    m_pageEffectSpeed = ES_MEDIUM;

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
        element.setAttribute("speed", static_cast<int>( m_pageEffectSpeed ));
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

QString KPBackGround::saveOasisPageEffect() const
{
    QString transition;
    switch( pageEffect )
    {
    case PEF_NONE:
        transition="none";
        break;
    case PEF_CLOSE_HORZ:
        transition="close-vertical";
        break;
    case PEF_CLOSE_VERT:
        transition="close-horizontal";
        break;
    case PEF_CLOSE_ALL:
        transition="close";
        break;
    case PEF_OPEN_HORZ:
        transition="open-vertical";
        break;
    case PEF_OPEN_VERT:
        transition="open-horizontal";
        break;
    case PEF_OPEN_ALL:
        transition="open";
        break;
    case PEF_INTERLOCKING_HORZ_1:
        transition="interlocking-horizontal-left";
        break;
    case PEF_INTERLOCKING_HORZ_2:
        transition="interlocking-horizontal-right";
        break;
    case PEF_INTERLOCKING_VERT_1:
        transition="interlocking-vertical-top";
        break;
    case PEF_INTERLOCKING_VERT_2:
        transition="interlocking-vertical-bottom";
        break;
    case PEF_SURROUND1:
        transition="spiralin-left";
        break;
    case PEF_FLY1:
        transition="fly-away";
        break;
    case PEF_BLINDS_HOR:
        transition="horizontal-stripes";
        break;
    case PEF_BLINDS_VER:
        transition="vertical-stripes";
        break;
    case PEF_BOX_IN:
        transition="fade-to-center";
        break;
    case PEF_BOX_OUT:
        transition="fade-from-center";
        break;
    case PEF_CHECKBOARD_ACROSS:
        transition="horizontal-checkerboard";
        break;
    case PEF_CHECKBOARD_DOWN:
        transition="vertical-checkerboard";
        break;
    case PEF_COVER_DOWN:
        transition="fade-from-top";
        break;
    case PEF_COVER_UP:
        transition="fade-from-bottom";
        break;
    case PEF_COVER_LEFT:
        transition="fade-from-right";
        break;
    case PEF_COVER_RIGHT:
        transition="fade-from-left";
        break;
    case PEF_COVER_LEFT_UP:
        transition="fade-from-lowerright";
        break;
    case PEF_COVER_LEFT_DOWN:
        transition="fade-from-upperright";
        break;
    case PEF_COVER_RIGHT_UP:
        transition="fade-from-lowerleft";
        break;
    case PEF_COVER_RIGHT_DOWN:
        transition="fade-from-upperleft";
        break;
    case PEF_UNCOVER_LEFT:
        transition="uncover-to-left";
        break;
    case PEF_UNCOVER_UP:
        transition="uncover-to-top";
        break;
    case PEF_UNCOVER_RIGHT:
        transition="uncover-to-right";
        break;
    case PEF_UNCOVER_DOWN:
        transition="uncover-to-bottom";
        break;
    case PEF_UNCOVER_LEFT_UP:
        transition="uncover-to-upperleft";
        break;
    case PEF_UNCOVER_LEFT_DOWN:
        transition="uncover-to-lowerleft";
        break;
    case PEF_UNCOVER_RIGHT_UP:
        transition="uncover-to-upperright";
        break;
    case PEF_UNCOVER_RIGHT_DOWN:
        transition="uncover-to-lowerright";
        break;
    case PEF_DISSOLVE:
        transition="dissolve";
        break;
    case PEF_STRIPS_LEFT_UP:
        transition="fade-from-lowerright";
        break;
    case PEF_STRIPS_LEFT_DOWN:
        transition="fade-from-upperright";
        break;
    case PEF_STRIPS_RIGHT_UP:
        transition="fade-from-lowerleft";
        break;
    case PEF_STRIPS_RIGHT_DOWN:
        transition="fade-from-upperleft";
        break;
    case PEF_MELTING:
        transition="melt";
        break;
    case PEF_LAST_MARKER://don't use it !!!
        break;
    case PEF_RANDOM:
        transition="random";
        break;

    }
    return transition;
}


QString KPBackGround::saveOasisBackgroundPageStyle( KoStore *store, KoXmlWriter &xmlWriter, KoGenStyles& mainStyles )
{
    KoGenStyle stylepageauto( KPresenterDoc::STYLE_BACKGROUNDPAGEAUTO, "drawing-page" );
    stylepageauto.addProperty( "presentation:background-visible", "true" ); //for the moment it's not implemented into kpresenter
    stylepageauto.addProperty( "presentation:background-objects-visible", "true" );
    QString transition = saveOasisPageEffect();
    if ( !transition.isEmpty() )
    {
        stylepageauto.addProperty( "presentation:transition-style", transition );
    }
    if ( pageTimer != 1 )
    {
        stylepageauto.addProperty("presentation:duration", saveOasisTimer( pageTimer ));
        //not used into kpresenter but necessary into ooimpress
        //keep compatible
        stylepageauto.addProperty( "presentation:transition-type", "automatic" );
    }
    if ( m_pageEffectSpeed != ES_MEDIUM ) // we don't save the default value
    {
        if ( m_pageEffectSpeed == ES_FAST )
            stylepageauto.addProperty( "presentation:transition-speed", "fast" );
        else if ( m_pageEffectSpeed == ES_SLOW )
            stylepageauto.addProperty( "presentation:transition-speed", "slow" );
    }
    if ( !m_page->isSlideSelected() )
        stylepageauto.addProperty( "presentation:visibility", "hidden" );
    if ( !soundFileName.isEmpty() && soundEffect )
    {
        QBuffer buffer;
        buffer.open( IO_WriteOnly );
        KoXmlWriter elementWriter( &buffer );  // TODO pass indentation level
        elementWriter.startElement( "presentation:sound" );
        elementWriter.addAttribute( "xlink:href", soundFileName );
        elementWriter.endElement();
        QString elementContents = QString::fromUtf8( buffer.buffer(), buffer.buffer().size() );
        stylepageauto.addChildElement( "sound effect", elementContents );
    }

    switch ( backType )
    {
    case BT_COLOR:
    {
        if ( bcType == BCT_PLAIN )
        {
            stylepageauto.addProperty( "draw:fill","solid" );
            stylepageauto.addProperty( "draw:fill-color", backColor1.name() );
        }
        else
        {
            stylepageauto.addProperty( "draw:fill","gradient" );
            stylepageauto.addProperty( "draw:fill-gradient-name", saveOasisGradientStyle( mainStyles ) );
        }
        break;
    }
    case BT_BRUSH:
    case BT_CLIPART:
    case BT_PICTURE:
        stylepageauto.addProperty("draw:fill", "bitmap" );
        stylepageauto.addProperty("draw:fill-image-name", saveOasisPictureStyle( mainStyles ) );
        //todo
        break;
    }

    return mainStyles.lookup( stylepageauto, "dp" );
}

QString KPBackGround::saveOasisPictureStyle( KoGenStyles& mainStyles )
{
    //<draw:fill-image draw:name="Pattern" xlink:href="#Pictures/100000000000005E0000005E43C87AF2.png" xlink:type="simple" xlink:show="embed" xlink:actuate="onLoad"/>
    KoGenStyle pictureStyle( KPresenterDoc::STYLE_PICTURE /*no family name*/ );
    pictureStyle.addAttribute( "xlink:show", "embed" );
    pictureStyle.addAttribute( "xlink:actuate", "onLoad" );
    pictureStyle.addAttribute( "xlink:type", "simple" );
    pictureStyle.addAttribute( "xlink:href","#"+ pictureCollection()->getOasisFileName(backPicture ) );
    return mainStyles.lookup( pictureStyle, "picture" );
}

QString KPBackGround::saveOasisGradientStyle( KoGenStyles& mainStyles )
{
    KoGenStyle gradientStyle( KPresenterDoc::STYLE_GRADIENT /*no family name*/);
    gradientStyle.addAttribute( "draw:start-color", backColor1.name() );
    gradientStyle.addAttribute( "draw:end-color", backColor2.name() );
    QString unbalancedx( "50%" );
    QString unbalancedy( "50%" );

    if ( unbalanced )
    {
        unbalancedx = QString( "%1%" ).arg( xfactor / 4 + 50 );
        unbalancedy = QString( "%1%" ).arg( yfactor / 4 + 50 );
    }
    gradientStyle.addAttribute( "draw:cx", unbalancedx );
    gradientStyle.addAttribute( "draw:cy", unbalancedy );


    switch( bcType )
    {
    case BCT_PLAIN:
        gradientStyle.addAttribute( "draw:angle", 0 );
        gradientStyle.addAttribute( "draw:style", "linear" );
        break;
    case BCT_GHORZ:
        gradientStyle.addAttribute( "draw:angle", 0 );
        gradientStyle.addAttribute( "draw:style", "linear" );
        break;
    case BCT_GVERT:
        gradientStyle.addAttribute( "draw:angle", 900 );
        gradientStyle.addAttribute( "draw:style", "linear" );
        break;
    case BCT_GDIAGONAL1:
        gradientStyle.addAttribute( "draw:angle", 450 );
        gradientStyle.addAttribute( "draw:style", "linear" );
        break;
    case BCT_GDIAGONAL2:
        gradientStyle.addAttribute( "draw:angle", 135 );
        gradientStyle.addAttribute( "draw:style", "linear" );
        break;
    case BCT_GCIRCLE:
        gradientStyle.addAttribute( "draw:angle", 0 );
        gradientStyle.addAttribute( "draw:style", "radial" );
        break;
    case BCT_GRECT:
        gradientStyle.addAttribute( "draw:angle", 0 );
        gradientStyle.addAttribute( "draw:style", "square" );
        break;
    case BCT_GPIPECROSS:
        gradientStyle.addAttribute( "draw:angle", 0 );
        gradientStyle.addAttribute( "draw:style", "axial" );
        break;
    case BCT_GPYRAMID: //todo fixme ! it doesn't work !
        gradientStyle.addAttribute( "draw:angle", 0 );
        gradientStyle.addAttribute( "draw:style", 0 );
        break;
    }
    return mainStyles.lookup( gradientStyle, "gradient" );
}

void KPBackGround::loadOasis(KoOasisContext & context )
{
    KoStyleStack& styleStack = context.styleStack();
    kdDebug()<<"KPBackGround::loadOasis()\n";
    styleStack.setTypeProperties( "drawing-page" );
    if ( styleStack.hasAttribute( "presentation:visibility" ) )
    {
        QString str =  styleStack.attribute( "presentation:visibility" );
        if ( str=="hidden" )
            m_page->slideSelected( false );
        else
            kdDebug()<<" presentation:visibility parameter not implemented :"<<str<<endl;
    }

    if ( styleStack.hasAttribute( "draw:fill" ) )
    {
        const QString fill = styleStack.attribute( "draw:fill" );
        kdDebug()<<"fill page  type :"<<fill<<endl;
        if ( fill == "solid" )
        {
            setBackColor1(QColor(styleStack.attribute( "draw:fill-color" ) ) );
            setBackColorType(BCT_PLAIN);
            setBackType(BT_COLOR);
        }
        else if ( fill == "bitmap" )
        {
            QString style = styleStack.attribute( "draw:fill-image-name" );
            QDomElement* draw =context.oasisStyles().drawStyles()[style];

            const QString href( draw->attribute("xlink:href") );
            kdDebug()<<" href: "<<href<<endl;
            if ( !href.isEmpty() && href[0] == '#' )
            {
                QString strExtension;
                const int result=href.findRev(".");
                if (result>=0)
                {
                    strExtension=href.mid(result+1); // As we are using KoPicture, the extension should be without the dot.
                }
                QString filename(href.mid(1));
                const KoPictureKey key(filename, QDateTime::currentDateTime(Qt::UTC));
                backPicture.setKey(key);

                KoStore* store = context.store();
                if ( store->open( filename ) )
                {
                    KoStoreDevice dev(store);
                    if ( !backPicture.load( &dev, strExtension ) )
                        kdWarning() << "Cannot load picture: " << filename << " " << href << endl;
                    store->close();
                }
                pictureCollection()->insertPicture( key, backPicture );
            }

            if ( styleStack.hasAttribute( "style:repeat" ) )
            {
                QString repeat = styleStack.attribute( "style:repeat" );
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
        else if ( fill == "gradient" )
        {
            QString style = styleStack.attribute( "draw:fill-gradient-name" );
            QDomElement *draw = context.oasisStyles().drawStyles()[style];
            if ( draw )
            {
                //kdDebug()<<" draw style : name :"<<style<<endl;
                setBackColor1( QColor( draw->attribute( "draw:start-color" ) ) );
                setBackColor2( QColor( draw->attribute( "draw:end-color" ) ) );
                setBackColorType( BCT_PLAIN );
                setBackType(BT_COLOR);
                QString type = draw->attribute( "draw:style" );
                if ( type == "linear" )
                {
                    int angle = draw->attribute( "draw:angle" ).toInt() / 10;

                    // make sure the angle is between 0 and 359
                    angle = abs( angle );
                    angle -= ( (int) ( angle / 360 ) ) * 360;

                    // What we are trying to do here is to find out if the given
                    // angle belongs to a horizontal, vertical or diagonal gradient.
                    int lower, upper, nearAngle = 0;
                    for ( lower = 0, upper = 45; upper < 360; lower += 45, upper += 45 )
                    {
                        if ( upper >= angle )
                        {
                            int distanceToUpper = abs( angle - upper );
                            int distanceToLower = abs( angle - lower );
                            nearAngle = distanceToUpper > distanceToLower ? lower : upper;
                            break;
                        }
                    }

                    // nearAngle should now be one of: 0, 45, 90, 135, 180...
                    if ( nearAngle == 0 || nearAngle == 180 )
                        setBackColorType(BCT_GHORZ);
                    else if ( nearAngle == 90 || nearAngle == 270 )
                        setBackColorType(BCT_GVERT);
                    else if ( nearAngle == 45 || nearAngle == 225 )
                        setBackColorType(BCT_GDIAGONAL1);
                    else if ( nearAngle == 135 || nearAngle == 315 )
                        setBackColorType(BCT_GDIAGONAL2);
                }
                else if ( type == "radial" || type == "ellipsoid" )
                    setBackColorType(BCT_GCIRCLE);
                else if ( type == "square" || type == "rectangular" )
                    setBackColorType(BCT_GRECT);
                else if ( type == "axial" )
                    setBackColorType(BCT_GPIPECROSS);


                // Hard to map between x- and y-center settings of ooimpress
                // and (un-)balanced settings of kpresenter. Let's try it.
                int x, y;
                if ( draw->hasAttribute( "draw:cx" ) )
                    x = draw->attribute( "draw:cx" ).remove( '%' ).toInt();
                else
                    x = 50;

                if ( draw->hasAttribute( "draw:cy" ) )
                    y = draw->attribute( "draw:cy" ).remove( '%' ).toInt();
                else
                    y = 50;

                if ( x == 50 && y == 50 )
                {
                    unbalanced=0;
                    setBackUnbalanced(false);
                    setBackXFactor(100);
                    setBackYFactor(100);
                }
                else
                {
                    unbalanced=1;
                    setBackUnbalanced(true);

                    // map 0 - 100% to -200 - 200
                    setBackXFactor(( 4 * x - 200 ));
                    setBackYFactor(( 4 * y - 200 ));
                }
            }
        }
    }
    if ( styleStack.hasAttribute( "presentation:transition-speed" ) )
    {
        // this argument is not defined into kpresenter_doc and not into kprpage
        // TODO add it into each page.
        QString speed = styleStack.attribute( "presentation:transition-speed" );
        if ( speed == "slow" )
        {
            m_pageEffectSpeed = ES_SLOW;
        }
        else if ( speed == "medium" )
        {
            m_pageEffectSpeed = ES_MEDIUM;
        }
        else if ( speed == "fast" )
        {
            m_pageEffectSpeed = ES_FAST;
        }
        else
            kdDebug()<<" transition-speed not defined :"<<speed<<endl;
    }
    if ( styleStack.hasAttribute("presentation:duration" ))
    {
        pageTimer = loadOasisTimer( styleStack.attribute("presentation:duration") );
    }
    if ( styleStack.hasAttribute( "presentation:transition-type" ) )
    {
        //Not defined into kpresenter
        //it's global for the moment.
        kdDebug()<<" presentation:transition-type :"<<styleStack.attribute( "presentation:transition-type" )<<endl;
    }
    if ( styleStack.hasAttribute("presentation:transition-style"))
    {
        kdDebug()<<" have a presentation:transition-style------------\n";
        const QString effect = styleStack.attribute("presentation:transition-style");
        kdDebug() << "Transition name: " << effect << endl;
        PageEffect pef;
        if ( effect=="none" )
            pef=PEF_NONE;
        else if (effect=="vertical-stripes" || effect=="vertical-lines") // PEF_BLINDS_VER
            pef=PEF_BLINDS_VER;
        else if (effect=="horizontal-stripes" || effect=="horizontal-lines") // PEF_BLINDS_HOR
            pef=PEF_BLINDS_HOR;
        else if (effect=="spiralin-left" || effect=="spiralin-right"
                 || effect== "spiralout-left" || effect=="spiralout-right") // PEF_SURROUND1
            pef=PEF_SURROUND1;
        else if (effect=="fade-from-upperleft") // PEF_STRIPS_RIGHT_DOWN
            pef=PEF_STRIPS_RIGHT_DOWN;
        else if (effect=="fade-from-upperright") // PEF_STRIPS_LEFT_DOWN
            pef=PEF_STRIPS_LEFT_DOWN;
        else if (effect=="fade-from-lowerleft") // PEF_STRIPS_RIGHT_UP
            pef=PEF_STRIPS_RIGHT_UP;
        else if (effect=="fade-from-lowerright") // PEF_STRIPS_LEFT_UP
            pef=PEF_STRIPS_LEFT_UP;
        else if (effect=="fade-from-top") // PEF_COVER_DOWN
            pef=PEF_COVER_DOWN;
        else if (effect=="fade-from-bottom") // PEF_COVER_UP
            pef=PEF_COVER_UP;
        else if (effect=="fade-from-left") // PEF_COVER_RIGHT
            pef=PEF_COVER_RIGHT;
        else if (effect=="fade-from-right") // PEF_COVER_LEFT
            pef=PEF_COVER_LEFT;
        else if (effect=="fade-from-lowerleft") // PEF_COVER_RIGHT_UP
            pef=PEF_COVER_RIGHT_UP;
        else if (effect=="fade-from-lowerright") // PEF_COVER_LEFT_UP
            pef=PEF_COVER_LEFT_UP;
        else if (effect=="fade-from-upperleft") // PEF_COVER_RIGHT_DOWN
            pef=PEF_COVER_RIGHT_DOWN;
        else if (effect=="fade-from-upperright") // PEF_COVER_LEFT_DOWN
            pef=PEF_COVER_LEFT_DOWN;
        else if (effect=="fade-to-center") // PEF_BOX_IN
            pef=PEF_BOX_IN;
        else if (effect=="fade-from-center") // PEF_BOX_OUT
            pef=PEF_BOX_OUT;
        else if (effect=="open-vertical") // PEF_OPEN_HORZ; really, no kidding ;)
            pef=PEF_OPEN_HORZ;
        else if (effect=="open-horizontal") // PEF_OPEN_VERT
            pef=PEF_OPEN_VERT;
        else if (effect=="open") // PEF_OPEN_ALL
            pef=PEF_OPEN_ALL;
        else if (effect=="close-vertical") // PEF_CLOSE_HORZ
            pef=PEF_CLOSE_HORZ;
        else if (effect=="close-horizontal") // PEF_CLOSE_VERT
            pef=PEF_CLOSE_VERT;
        else if (effect=="close") // PEF_CLOSE_ALL
            pef=PEF_CLOSE_ALL;
        else if (effect=="dissolve") // PEF_DISSOLVE; perfect hit ;)
            pef=PEF_DISSOLVE;
        else if (effect=="horizontal-checkerboard") // PEF_CHECKBOARD_ACROSS
            pef=PEF_CHECKBOARD_ACROSS;
        else if (effect=="vertical-checkerboard") // PEF_CHECKBOARD_DOWN
            pef=PEF_CHECKBOARD_DOWN;
        else if (effect=="uncover-to-right" || effect=="roll-from-left") // PEF_UNCOVER_RIGHT
            pef=PEF_UNCOVER_RIGHT;
        else if (effect=="uncover-to-left" || effect=="roll-from-right") // PEF_UNCOVER_LEFT
            pef=PEF_UNCOVER_LEFT;
        else if (effect=="uncover-to-top" || effect=="roll-from-bottom") // PEF_UNCOVER_UP
            pef=PEF_UNCOVER_UP;
        else if (effect=="uncover-to-bottom" || effect=="roll-from-top") // PEF_UNCOVER_DOWN
            pef=PEF_UNCOVER_DOWN;
        else if (effect=="uncover-to-upperleft") // PEF_UNCOVER_LEFT_UP
            pef=PEF_UNCOVER_LEFT_UP;
        else if (effect=="uncover-to-upperright") // PEF_UNCOVER_RIGHT_UP
            pef=PEF_UNCOVER_RIGHT_UP;
        else if (effect=="uncover-to-lowerleft") // PEF_UNCOVER_LEFT_DOWN
            pef=PEF_UNCOVER_LEFT_DOWN;
        else if (effect=="uncover-to-lowerright") // PEF_UNCOVER_RIGHT_DOWN
            pef=PEF_UNCOVER_RIGHT_DOWN;
        else if (effect=="interlocking-horizontal-left")
            pef=PEF_INTERLOCKING_HORZ_1;
        else if (effect=="interlocking-horizontal-right")
            pef=PEF_INTERLOCKING_HORZ_2;
        else if (effect=="interlocking-vertical-top")
            pef=PEF_INTERLOCKING_VERT_1;
        else if (effect=="interlocking-vertical-bottom")
            pef=PEF_INTERLOCKING_VERT_2;
        else if ( effect=="melt" )
            pef=PEF_MELTING;
        else if ( effect=="fly-away" )
            pef=PEF_FLY1;
        else if ( effect=="random" )
            pef=PEF_RANDOM;
        else         // we choose a random transition instead of the unsupported ones ;)
            pef=PEF_RANDOM;
        setPageEffect( pef );
    }
    if ( styleStack.hasChildNode("presentation:sound"))
    {
        kdDebug()<<" presentation:sound !!!!!!!!!!!!!!!!!!!!!\n";
        QDomElement sound = styleStack.childNode("presentation:sound").toElement();
        soundEffect = true;
        soundFileName = sound.attribute( "xlink:href" );
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
        if(e.hasAttribute("speed"))
            tmp=e.attribute("speed").toInt();
        setPageEffectSpeed( static_cast<EffectSpeed>(tmp) );
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
