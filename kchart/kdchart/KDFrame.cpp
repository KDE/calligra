/* -*- Mode: C++ -*-
   KD Tools - a set of useful widgets for Qt
*/

/****************************************************************************
** Copyright (C) 2001-2003 Klarälvdalens Datakonsult AB.  All rights reserved.
**
** This file is part of the KD Tools library.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid commercial KD Tools licenses may use this file in
** accordance with the KD Tools Commercial License Agreement provided with
** the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.klaralvdalens-datakonsult.se/?page=products for
**   information about KD Tools Commercial License Agreements.
**
** Contact info@klaralvdalens-datakonsult.se if any conditions of this
** licensing are not clear to you.
**
**********************************************************************/

#include <qpainter.h>
#include <qbrush.h>
//Added by qt3to4:
#include <QPixmap>

#include <KDFrame.h>
#include <KDFrameProfileSection.h>
#include <KDXMLTools.h>

#if defined( SUN7 ) || defined( _SGIAPI )
#include <math.h>
#else
#include <cmath>
#endif
#ifdef Q_WS_WIN
#define M_PI 3.14159265358979323846
#endif


KDFrame::~KDFrame()
{
    _profileSections.clear(); // is ignored if auto-deletion is disabled
}

KDFrame::KDFrameCorner::~KDFrameCorner()
{
    // Intentionally left blank for now.
}





void KDFrame::paintBackground( QPainter& painter, const QRect& innerRect ) const
{
    /* first draw the brush (may contain a pixmap)*/
    if( Qt::NoBrush != _background.style() ) {
        QPen   oldPen(   painter.pen() );
        QPoint oldOrig(  painter.brushOrigin() );
        QBrush oldBrush( painter.brush() );
        painter.setPen( Qt::NoPen );
        const QPoint newTopLeft( innerRect.topLeft() * painter.matrix() );
        painter.setBrushOrigin( newTopLeft.x(), newTopLeft.y() );
        painter.setBrush( _background );
        painter.drawRect( innerRect.x(), innerRect.y(),
                          innerRect.width(), innerRect.height() );
        painter.setPen(         oldPen );
        painter.setBrushOrigin( oldOrig );
        painter.setBrush(       oldBrush );
    }
    /* next draw the backPixmap over the brush */
    if( ! _backPixmap.isNull() ) {
        QPoint ol = innerRect.topLeft();
        if( PixCentered == _backPixmapMode )
        {
            ol.setX( innerRect.center().x() - _backPixmap.width() / 2 );
            ol.setY( innerRect.center().y() - _backPixmap.height()/ 2 );
            bitBlt( painter.device(), ol, &_backPixmap );
        } else {
            QMatrix m;
            double zW = (double)innerRect.width()  / (double)_backPixmap.width();
            double zH = (double)innerRect.height() / (double)_backPixmap.height();
            switch ( _backPixmapMode ) {
                case PixCentered:
                    break;
                case PixScaled: {
                                    double z;
                                    z = qMin( zW, zH );
                                    m.scale( z, z );
                                }
                                break;
                case PixStretched:
                                m.scale( zW, zH );
                                break;
            }
            QPixmap pm = _backPixmap.transformed( m );
            ol.setX( innerRect.center().x() - pm.width() / 2 );
            ol.setY( innerRect.center().y() - pm.height()/ 2 );
            bitBlt( painter.device(), ol, &pm );
        }
    }
}


void KDFrame::paintEdges( QPainter& painter, const QRect& innerRect ) const
    {

/*
    Note: The following code OF COURSE is only dummy-code and will be replaced.

          At the moment it is used to draw the simple frames which were set by
          the setSimpleFrame() function.
*/
    if( !_topProfile.isEmpty() ){

        KDFrameProfileSection* section;
        for ( section = ((KDFrameProfile)_topProfile).last(); section; section = ((KDFrameProfile)_topProfile).prev() ){
            const QPen   oldPen   = painter.pen();
            const QBrush oldBrush = painter.brush();
            QPen thePen;
            thePen = section->pen();
            int penWidth = qMax(thePen.width(), 1) * qMax(section->width(), 1);
//qDebug("paintEdges: thePen.width() = %i", thePen.width());
//qDebug("paintEdges: section->width() = %i", section->width());
//qDebug("paintEdges: penWidth = %i", penWidth);
            thePen.setWidth( penWidth );
            painter.setPen( thePen );
            painter.setBrush( Qt::NoBrush );
            painter.drawRect( innerRect.x()-penWidth,
                              innerRect.y()-penWidth,
                              innerRect.width()  +2*penWidth,
                              innerRect.height() +2*penWidth );
            painter.setBrush( oldBrush );
            painter.setPen( oldPen );
        }
    }
}


void KDFrame::paintCorners( QPainter& /*painter*/, const QRect& /*innerRect*/ ) const
{
    // At the moment nothing is done here since the setSimpleFrame() function
    // does not allow specifying corners: all predefined frames have normal
    // (default) corners.
    ;
}


void KDFrame::paint( QPainter* painter,
        KDFramePaintSteps steps,
        QRect innerRect ) const
{
    if( painter ) {
        const QRect& rect = innerRect.isValid() ? innerRect : _innerRect;
        switch( steps ) {
            case PaintBackground:
                paintBackground( *painter, rect );
                break;
            case PaintEdges:
                paintEdges(      *painter, rect );
                break;
            case PaintCorners:
                paintCorners(    *painter, rect );
                break;
            case PaintBorder:
                paintEdges(      *painter, rect );
                paintCorners(    *painter, rect );
                break;
            case PaintAll:
                paintBackground( *painter, rect );
                paintEdges(      *painter, rect );
                paintCorners(    *painter, rect );
                break;
        }
    }
}


void KDFrame::clearProfile( ProfileName name )
{
    switch( name ) {
        case ProfileTop:   _topProfile.clear();
                           break;
        case ProfileRight: _rightProfile.clear();
                           break;
        case ProfileBottom:_bottomProfile.clear();
                           break;
        case ProfileLeft:  _leftProfile.clear();
                           break;
    }
}

void KDFrame::addProfileSection( ProfileName      name,
                                 int              wid,
                                 QPen             pen,
                                 KDFrameProfileSection::Direction dir,
                                 KDFrameProfileSection::Curvature curv )
{
    switch( name ) {
        case ProfileTop:   _topProfile.append(   new KDFrameProfileSection( dir, curv, wid, pen ) );
                           break;
        case ProfileRight: _rightProfile.append( new KDFrameProfileSection( dir, curv, wid, pen ) );
                           break;
        case ProfileBottom:_bottomProfile.append(new KDFrameProfileSection( dir, curv, wid, pen ) );
                           break;
        case ProfileLeft:  _leftProfile.append(  new KDFrameProfileSection( dir, curv, wid, pen ) );
                           break;
    }
}

void KDFrame::setProfile( ProfileName name, const KDFrameProfile& profile )
{
    switch( name ) {
        case ProfileTop:   _topProfile    = profile;
                           break;
        case ProfileRight: _rightProfile  = profile;
                           break;
        case ProfileBottom:_bottomProfile = profile;
                           break;
        case ProfileLeft:  _leftProfile   = profile;
                           break;
    }
}

const KDFrameProfile& KDFrame::profile( ProfileName name ) const
{
    switch( name ) {
        case ProfileTop:   return _topProfile;
                           break;
        case ProfileRight: return _rightProfile;
                           break;
        case ProfileBottom:return _bottomProfile;
                           break;
        default:           return _leftProfile;
    }

    return _leftProfile;
}


void KDFrame::setSimpleFrame( SimpleFrame frame,
                              int         lineWidth,
                              int         midLineWidth,
                              QPen        pen,
                              QBrush      background,
                              const QPixmap* backPixmap,
                              BackPixmapMode backPixmapMode )
{
    _profileSections.clear();
    _topProfile.clear();
    _rightProfile.clear();
    _bottomProfile.clear();
    _leftProfile.clear();
    _background = background;
    _backPixmap = backPixmap ? *backPixmap : QPixmap();
    _backPixmapMode = backPixmapMode;
    if( FrameFlat == frame ) {
        //qDebug("_profileSections.count() before = %i", _profileSections.count());
        KDFrameProfileSection* newsection =
            new KDFrameProfileSection( KDFrameProfileSection::DirPlain,
                    KDFrameProfileSection::CvtPlain,
                    lineWidth, pen );
        _profileSections.append( newsection );
        //qDebug( "_profileSections.count() after = %i,    lineWidth = %i",
        //        _profileSections.count(),
        //        lineWidth );
        _topProfile.append( newsection );
        _rightProfile  = _topProfile;
        _bottomProfile = _topProfile;
        _leftProfile   = _topProfile;
    }
    else {
        switch( frame ) {
            case FrameElegance: {
                                    int line1 = lineWidth / 8;
                                    int line2 = lineWidth / 16;
                                    int line3 = line2;
                                    int gap2  = line2 * 4;
                                    int gap1  = lineWidth - line1 - line2 - line3 - gap2;
                                    QPen noP( Qt::NoPen );
                                    KDFrameProfileSection* newsection;
                                    newsection = new KDFrameProfileSection( KDFrameProfileSection::DirPlain,
                                            KDFrameProfileSection::CvtPlain,
                                            line1, pen );
                                    _profileSections.append( newsection );
                                    _topProfile.append( newsection );
                                    newsection = new KDFrameProfileSection( KDFrameProfileSection::DirPlain,
                                            KDFrameProfileSection::CvtPlain,
                                            gap1,  noP );
                                    _profileSections.append( newsection );
                                    _topProfile.append( newsection );
                                    newsection = new KDFrameProfileSection( KDFrameProfileSection::DirPlain,
                                            KDFrameProfileSection::CvtPlain,
                                            line2, pen );
                                    _profileSections.append( newsection );
                                    _topProfile.append( newsection );
                                    newsection = new KDFrameProfileSection( KDFrameProfileSection::DirPlain,
                                            KDFrameProfileSection::CvtPlain,
                                            gap2,  noP );
                                    _profileSections.append( newsection );
                                    _topProfile.append( newsection );
                                    newsection =  new KDFrameProfileSection( KDFrameProfileSection::DirPlain,
                                            KDFrameProfileSection::CvtPlain,
                                            line3, pen );
                                    _profileSections.append( newsection );
                                    _topProfile.append( newsection );
                                }
                                break;
            case FrameBoxRaized:
                                {
                                    KDFrameProfileSection* newsection;
                                    newsection = new KDFrameProfileSection( KDFrameProfileSection::DirRaising,
                                            KDFrameProfileSection::CvtPlain,
                                            lineWidth, pen );
                                    _profileSections.append( newsection );
                                    _topProfile.append( newsection );
                                    newsection = new KDFrameProfileSection( KDFrameProfileSection::DirPlain,
                                            KDFrameProfileSection::CvtPlain,
                                            midLineWidth, pen );
                                    _profileSections.append( newsection );
                                    _topProfile.append( newsection );
                                    newsection =  new KDFrameProfileSection( KDFrameProfileSection::DirSinking,
                                            KDFrameProfileSection::CvtPlain,
                                            lineWidth, pen );
                                    _profileSections.append( newsection );
                                    _topProfile.append( newsection );
                                    break;
                                }
            case FrameBoxSunken:
                                {
                                    KDFrameProfileSection* newsection;
                                    newsection = new KDFrameProfileSection( KDFrameProfileSection::DirSinking,
                                            KDFrameProfileSection::CvtPlain,
                                            lineWidth, pen );
                                    _profileSections.append( newsection );
                                    _topProfile.append( newsection );
                                    newsection = new KDFrameProfileSection( KDFrameProfileSection::DirPlain,
                                            KDFrameProfileSection::CvtPlain,
                                            midLineWidth, pen );
                                    _profileSections.append( newsection );
                                    _topProfile.append( newsection );
                                    newsection = new KDFrameProfileSection( KDFrameProfileSection::DirRaising,
                                            KDFrameProfileSection::CvtPlain,
                                            lineWidth, pen );
                                    _profileSections.append( newsection );
                                    _topProfile.append( newsection );
                                }
                                break;
            case FramePanelRaized:
                                {
                                    KDFrameProfileSection* newsection;
                                    newsection =  new KDFrameProfileSection( KDFrameProfileSection::DirRaising,
                                            KDFrameProfileSection::CvtPlain,
                                            lineWidth, pen );
                                    _profileSections.append( newsection );
                                    _topProfile.append( newsection );
                                    break;
                                }
            case FramePanelSunken:
                                {
                                    KDFrameProfileSection* newsection;
                                    newsection =  new KDFrameProfileSection( KDFrameProfileSection::DirSinking,
                                            KDFrameProfileSection::CvtPlain,
                                            lineWidth, pen );
                                    _profileSections.append( newsection );
                                    _topProfile.append( newsection );
                                }
                                break;
            case FrameSemicircular:
                                {
                                    KDFrameProfileSection* newsection;
                                    newsection = new KDFrameProfileSection( KDFrameProfileSection::DirRaising,
                                            KDFrameProfileSection::CvtConvex,
                                            lineWidth, pen );
                                    _profileSections.append( newsection );
                                    _topProfile.append( newsection );
                                    newsection =  new KDFrameProfileSection( KDFrameProfileSection::DirPlain,
                                            KDFrameProfileSection::CvtPlain,
                                            midLineWidth, pen );
                                    _profileSections.append( newsection );
                                    _topProfile.append( newsection );
                                    newsection = new KDFrameProfileSection( KDFrameProfileSection::DirSinking,
                                            KDFrameProfileSection::CvtConcave,
                                            lineWidth, pen );
                                    _profileSections.append( newsection );
                                    _topProfile.append( newsection );
                                }
                                break;
            default:
                                break;
        }
    }
    _rightProfile  = _topProfile;
    _bottomProfile = _topProfile;
    _leftProfile   = _topProfile;
    setCorners( CornerNormal, 0 );
}


void KDFrame::createFrameNode( QDomDocument& document, QDomNode& parent,
        const QString& elementName,
        const KDFrame& frame )
{
    QDomElement frameElement = document.createElement( elementName );
    parent.appendChild( frameElement );
    KDXML::createIntNode( document, frameElement, "ShadowWidth",
            frame._shadowWidth );
    KDXML::createStringNode( document, frameElement, "CornerName",
            cornerNameToString( frame._sunPos ) );

    KDXML::createBrushNode( document, frameElement, "Background",
            frame._background );
    KDXML::createPixmapNode( document, frameElement, "BackPixmap",
            frame._backPixmap );
    KDXML::createStringNode( document, frameElement, "BackPixmapMode",
            backPixmapModeToString( frame._backPixmapMode ) );

    KDXML::createRectNode( document, frameElement, "InnerRect",
            frame._innerRect );
    createFrameProfileNode( document, frameElement, "TopProfile",
            frame._topProfile );
    createFrameProfileNode( document, frameElement, "RightProfile",
            frame._rightProfile );
    createFrameProfileNode( document, frameElement, "BottomProfile",
            frame._bottomProfile );
    createFrameProfileNode( document, frameElement, "LeftProfile",
            frame._leftProfile );
    KDFrameCorner::createFrameCornerNode( document, frameElement, "CornerTL",
            frame._cornerTL );
    KDFrameCorner::createFrameCornerNode( document, frameElement, "CornerTR",
            frame._cornerTR );
    KDFrameCorner::createFrameCornerNode( document, frameElement, "CornerBL",
            frame._cornerBL );
    KDFrameCorner::createFrameCornerNode( document, frameElement, "CornerBR",
            frame._cornerBR );
}

void KDFrame::createFrameProfileNode( QDomDocument& document, QDomNode& parent,
        const QString& elementName,
        KDFrameProfile profile )
{
    QDomElement profileElement = document.createElement( elementName );
    parent.appendChild( profileElement );
    for( const KDFrameProfileSection* section = profile.first(); section != 0;
            section = profile.next() )
        KDFrameProfileSection::createFrameProfileSectionNode( document,
                profileElement,
                "ProfileSection",
                section );
}


void KDFrame::KDFrameCorner::createFrameCornerNode( QDomDocument& document,
        QDomNode& parent,
        const QString& elementName,
        const KDFrameCorner& corner )
{
    QDomElement cornerElement = document.createElement( elementName );
    parent.appendChild( cornerElement );
    KDXML::createStringNode( document, cornerElement, "Style",
            KDFrame::cornerStyleToString( corner._style ) );
    KDXML::createIntNode( document, cornerElement, "Width",
            corner._width );
    createFrameProfileNode( document, cornerElement, "Profile",
            corner._profile );
}

bool KDFrame::readFrameNode( const QDomElement& element, KDFrame& frame )
{
    bool ok = true;
    int tempShadowWidth;
    CornerName tempCornerName=CornerTopLeft;
    QBrush tempBackground;
    QPixmap tempBackPixmap;
    BackPixmapMode tempBackPixmapMode=PixStretched;
    QRect tempInnerRect;
    KDFrameProfile tempTopProfile, tempRightProfile,
    tempBottomProfile, tempLeftProfile;
    KDFrameCorner tempCornerTL, tempCornerTR, tempCornerBL, tempCornerBR;
    QDomNode node = element.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "ShadowWidth" ) {
                ok = ok & KDXML::readIntNode( element, tempShadowWidth );
            } else if( tagName == "CornerName" ) {
                QString value;
                ok = ok & KDXML::readStringNode( element, value );
                tempCornerName = stringToCornerName( value );
            } else if( tagName == "Background" ) {
                ok = ok & KDXML::readBrushNode( element, tempBackground );
            } else if( tagName == "BackPixmap" ) {
                ok = ok & KDXML::readPixmapNode( element, tempBackPixmap );
            } else if( tagName == "BackPixmapMode" ) {
                QString value;
                ok = ok & KDXML::readStringNode( element, value );
                tempBackPixmapMode = stringToBackPixmapMode( value );
            } else if( tagName == "InnerRect" ) {
                ok = ok & KDXML::readRectNode( element, tempInnerRect );
            } else if( tagName == "TopProfile" ) {
                ok = ok & readFrameProfileNode( element, tempTopProfile );
            } else if( tagName == "RightProfile" ) {
                ok = ok & readFrameProfileNode( element, tempRightProfile );
            } else if( tagName == "BottomProfile" ) {
                ok = ok & readFrameProfileNode( element, tempBottomProfile );
            } else if( tagName == "LeftProfile" ) {
                ok = ok & readFrameProfileNode( element, tempLeftProfile );
            } else if( tagName == "CornerTL" ) {
                ok = ok & KDFrameCorner::readFrameCornerNode( element,
                        tempCornerTL );
            } else if( tagName == "CornerTR" ) {
                ok = ok & KDFrameCorner::readFrameCornerNode( element,
                        tempCornerTR );
            } else if( tagName == "CornerBL" ) {
                ok = ok & KDFrameCorner::readFrameCornerNode( element,
                        tempCornerBL );
            } else if( tagName == "CornerBR" ) {
                ok = ok & KDFrameCorner::readFrameCornerNode( element,
                        tempCornerBR );
            } else {
                qDebug( "Unknown tag in frame" );
            }
        }
        node = node.nextSibling();
    }

    if( ok ) {
        frame._shadowWidth = tempShadowWidth;
        frame._sunPos = tempCornerName;
        frame._background = tempBackground;
        frame._backPixmap = tempBackPixmap;
        frame._backPixmapMode = tempBackPixmapMode;
        frame._innerRect = tempInnerRect;
        frame._topProfile = tempTopProfile;
        frame._rightProfile = tempRightProfile;
        frame._bottomProfile = tempBottomProfile;
        frame._leftProfile = tempLeftProfile;
        frame._cornerTL = tempCornerTL;
        frame._cornerTR = tempCornerTR;
        frame._cornerBL = tempCornerBL;
        frame._cornerBR = tempCornerBR;
    }

    return ok;
}


bool KDFrame::readFrameProfileNode( const QDomElement& element,
        KDFrameProfile& profile )
{
    QDomNode node = element.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "ProfileSection" ) {
                KDFrameProfileSection* section = new KDFrameProfileSection();
                KDFrameProfileSection::readFrameProfileSectionNode( element,
                        section );
                profile.append( section );
            } else {
                qDebug( "Unknown tag in double map" );
                return false;
            }
        }
        node = node.nextSibling();
    }

    return true;
}


bool KDFrame::KDFrameCorner::readFrameCornerNode( const QDomElement& element,
        KDFrameCorner& corner )
{
    bool ok = true;
    CornerStyle tempStyle=CornerNormal;
    int tempWidth;
    KDFrameProfile tempProfile;
    QDomNode node = element.firstChild();
    while( !node.isNull() ) {
        QDomElement element = node.toElement();
        if( !element.isNull() ) { // was really an element
            QString tagName = element.tagName();
            if( tagName == "Style" ) {
                QString value;
                ok = ok & KDXML::readStringNode( element, value );
                tempStyle = stringToCornerStyle( value );
            } else if( tagName == "Width" ) {
                ok = ok & KDXML::readIntNode( element, tempWidth );
            } else if( tagName == "Profile" ) {
                KDFrameProfile profile;
                ok = ok & readFrameProfileNode( element, profile );
            } else {
                qDebug( "Unknown tag in frame" );
            }
        }
        node = node.nextSibling();
    }

    if( ok ) {
        corner._style = tempStyle;
        corner._width = tempWidth;
        corner._profile = tempProfile;
    }

    return ok;
}

#include "KDFrame.moc"
