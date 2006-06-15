// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005-2006 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrObjectAdaptor.h"

#include "KPrObject.h"
#include "KPrDocument.h"
#include "KPrUtils.h"

#include <qpainter.h>
#include <qmatrix.h>
#include <q3pointarray.h>
#include <qregion.h>
#include <qdom.h>
#include <qbuffer.h>
#include <QRegExp>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3CString>

#include <kapplication.h>
#include <KoOasisContext.h>
#include "KoOasisStyles.h"

#include <stdlib.h>
#include <fstream>
#include <math.h>
#include <float.h>

#include <KoTextZoomHandler.h>
#include <KoRect.h>
#include <KoSize.h>
#include <KoPoint.h>
#include <KoXmlNS.h>
#include <KoDom.h>
#include <kdebug.h>

const QString &KPrObject::tagORIG=KGlobal::staticQString("ORIG");
const QString &KPrObject::attrX=KGlobal::staticQString("x");
const QString &KPrObject::attrY=KGlobal::staticQString("y");
const QString &KPrObject::tagSIZE=KGlobal::staticQString("SIZE");
const QString &KPrObject::attrWidth=KGlobal::staticQString("width");
const QString &KPrObject::attrHeight=KGlobal::staticQString("height");
const QString &KPrObject::tagSHADOW=KGlobal::staticQString("SHADOW");
const QString &KPrObject::attrDistance=KGlobal::staticQString("distance");
const QString &KPrObject::attrDirection=KGlobal::staticQString("direction");
const QString &KPrObject::attrColor=KGlobal::staticQString("color");
const QString &KPrObject::tagEFFECTS=KGlobal::staticQString("EFFECTS");
const QString &KPrObject::attrEffect=KGlobal::staticQString("effect");
const QString &KPrObject::attrEffect2=KGlobal::staticQString("effect2");
const QString &KPrObject::tagPRESNUM=KGlobal::staticQString("PRESNUM");
const QString &KPrObject::tagANGLE=KGlobal::staticQString("ANGLE");
const QString &KPrObject::tagDISAPPEAR=KGlobal::staticQString("DISAPPEAR");
const QString &KPrObject::attrDoit=KGlobal::staticQString("doit");
const QString &KPrObject::attrNum=KGlobal::staticQString("num");
const QString &KPrObject::tagFILLTYPE=KGlobal::staticQString("FILLTYPE");
const QString &KPrObject::tagGRADIENT=KGlobal::staticQString("GRADIENT");
const QString &KPrObject::tagPEN=KGlobal::staticQString("PEN");
const QString &KPrObject::tagBRUSH=KGlobal::staticQString("BRUSH");
const QString &KPrObject::attrValue=KGlobal::staticQString("value");
const QString &KPrObject::attrC1=KGlobal::staticQString("color1");
const QString &KPrObject::attrC2=KGlobal::staticQString("color2");
const QString &KPrObject::attrType=KGlobal::staticQString("type");
const QString &KPrObject::attrUnbalanced=KGlobal::staticQString("unbalanced");
const QString &KPrObject::attrXFactor=KGlobal::staticQString("xfactor");
const QString &KPrObject::attrYFactor=KGlobal::staticQString("yfactor");
const QString &KPrObject::attrStyle=KGlobal::staticQString("style");

KPrStartEndLine::KPrStartEndLine( LineEnd _start, LineEnd _end )
    : lineBegin( _start ), lineEnd( _end )
{
}

void KPrStartEndLine::save( QDomDocumentFragment &fragment, QDomDocument& doc )
{
    if (lineBegin!=L_NORMAL)
        fragment.appendChild(KPrStartEndLine::createValueElement("LINEBEGIN", static_cast<int>(lineBegin), doc));
    if (lineEnd!=L_NORMAL)
        fragment.appendChild(KPrStartEndLine::createValueElement("LINEEND", static_cast<int>(lineEnd), doc));
}

QDomElement KPrStartEndLine::createValueElement(const QString &tag, int value, QDomDocument &doc)
{
    QDomElement elem=doc.createElement(tag);
    elem.setAttribute("value", value);
    return elem;
}


void KPrStartEndLine::load( const QDomElement &element )
{
    QDomElement e=element.namedItem("LINEBEGIN").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        lineBegin=static_cast<LineEnd>(tmp);
    }
    e=element.namedItem("LINEEND").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        lineEnd=static_cast<LineEnd>(tmp);
    }
}

void KPrStartEndLine::loadOasisMarkerElement( KoOasisContext & context, const QString & attr, LineEnd &_element )
{
    //kDebug()<<"void KPrStartEndLine::loadOasisMarkerElement( KoOasisContext & context, const QString & attr, LineEnd &_element ) :"<<attr<<endl;

    KoStyleStack &styleStack = context.styleStack();
    styleStack.setTypeProperties( "graphic" );
    if ( styleStack.hasAttributeNS( KoXmlNS::draw,attr.toLatin1() ) )
    {
        QString style = styleStack.attributeNS( KoXmlNS::draw, attr.toLatin1() );
        //kDebug()<<" marker style is  : "<<style<<endl;

        //type not defined by default
        //try to use style.
        QDomElement* draw = context.oasisStyles().drawStyles()[style];
        //kDebug()<<" marker have oasis style defined :"<<draw<<endl;
        if ( draw )
        {
            if( draw->hasAttributeNS( KoXmlNS::svg, "d" ))
            {
                QString str = draw->attributeNS( KoXmlNS::svg, "d", QString::null );
                kDebug()<<" svg type = "<<str<<endl;
                if ( str == lineEndBeginSvg( L_ARROW ) )
                    _element = L_ARROW;
                else if ( str == lineEndBeginSvg( L_CIRCLE ) )
                    _element = L_CIRCLE;
                else if ( str == lineEndBeginSvg( L_SQUARE ) )
                    _element = L_SQUARE;
                else if ( str == lineEndBeginSvg( L_LINE_ARROW ) )
                    _element = L_LINE_ARROW;
                else if ( str == lineEndBeginSvg( L_DIMENSION_LINE ) )
                    _element = L_DIMENSION_LINE;
                else if ( str == lineEndBeginSvg( L_DOUBLE_ARROW ) )
                    _element = L_DOUBLE_ARROW;
                else if ( str == lineEndBeginSvg( L_DOUBLE_LINE_ARROW ) )
                    _element = L_DOUBLE_LINE_ARROW;
                else
                {
                    kDebug()<<" element not defined :"<<str<<endl;
                    _element = L_NORMAL;
                }
            }
        }
    }
}

void KPrStartEndLine::saveOasisMarkerElement( KoGenStyles& mainStyles,  KoGenStyle &styleobjectauto ) const
{
    if ( lineBegin != L_NORMAL )
    {
        styleobjectauto.addProperty( "draw:marker-start", saveOasisMarkerStyle( mainStyles, lineBegin ) );
        styleobjectauto.addProperty( "draw:marker-start-width", "0.25cm" );//value from ooimpress
    }
    if ( lineEnd != L_NORMAL )
    {
        styleobjectauto.addProperty( "draw:marker-end", saveOasisMarkerStyle( mainStyles, lineEnd ) );
        styleobjectauto.addProperty( "draw:marker-end-width", "0.25cm" );
    }
}

QString KPrStartEndLine::saveOasisMarkerStyle( KoGenStyles &mainStyles, const LineEnd &_element ) const
{
    KoGenStyle marker( KPrDocument::STYLE_MARKER /*, "graphic"*/ /*no name*/ );
    // value used from ooimpress filter I don't know if it's good
    switch( _element )
    {
    case L_NORMAL:
        //nothing
        break;
    case L_ARROW:
        marker.addAttribute( "svg:viewBox", "0 0 20 30" );
        break;
    case L_SQUARE:
        marker.addAttribute( "svg:viewBox", "0 0 10 10" );
        break;
    case L_CIRCLE:
        marker.addAttribute( "svg:viewBox", "0 0 1131 1131" );
        break;
    case L_LINE_ARROW:
        marker.addAttribute( "svg:viewBox", "0 0 1122 2243" );
        break;
    case L_DIMENSION_LINE:
        marker.addAttribute( "svg:viewBox", "0 0 836 110" );
        break;
    case L_DOUBLE_ARROW:
        marker.addAttribute( "svg:viewBox", "0 0 1131 1918" );
        break;
    case L_DOUBLE_LINE_ARROW:
        //FIXME !!!!!!!!!!!!
        // not defined into ooimpress filter.
        //marker.addAttribute( "svg:viewBox", "...." );
        //marker.addAttribute( "svg:d", "...." );
        break;
    }
    marker.addAttribute( "svg:d", lineEndBeginSvg( _element ));

    return mainStyles.lookup( marker, "marker" );
}

KPrObject::KPrObject()
    : orig(), ext(), shadowColor( Qt::gray )
{
    appearStep = 0;
    disappearStep = 1;
    effect = EF_NONE;
    effect2 = EF2_NONE;
    effect3 = EF3_NONE;
    m_appearSpeed = ES_MEDIUM;
    m_disappearSpeed = ES_MEDIUM;
    disappear = false;
    appearTimer = 1;
    disappearTimer = 1;
    appearSoundEffect = false;
    disappearSoundEffect = false;
    a_fileName = QString::null;
    d_fileName = QString::null;
    m_objectName = QString::null;
    angle = 0.0;
    shadowDirection = SD_RIGHT_BOTTOM;
    shadowDistance = 0;
    selected = false;
    ownClipping = true;
    subPresStep = 0;
    specEffects = false;
    onlyCurrStep = true;
    inObjList = true;
    cmds = 0;
    resize = false;
    protect = false;
    keepRatio = false;
    dbus= new KPrObjectAdaptor( this );
}

KPrObject::~KPrObject()
{
}

QDomDocumentFragment KPrObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment=doc.createDocumentFragment();
    QDomElement elem=doc.createElement(tagORIG);
    elem.setAttribute(attrX, orig.x());
    elem.setAttribute(attrY, QString::number( orig.y() + offset, 'g', DBL_DIG ) );
    fragment.appendChild(elem);
    elem=doc.createElement(tagSIZE);
    elem.setAttribute(attrWidth, ext.width());
    elem.setAttribute(attrHeight, ext.height());
    fragment.appendChild(elem);
    if(shadowDistance!=0 || shadowDirection!=SD_RIGHT_BOTTOM || shadowColor!=Qt::gray) {
        elem=doc.createElement(tagSHADOW);
        elem.setAttribute(attrDistance, shadowDistance);
        elem.setAttribute(attrDirection, static_cast<int>( shadowDirection ));
        elem.setAttribute(attrColor, shadowColor.name());
        fragment.appendChild(elem);
    }
    if(effect!=EF_NONE || effect2!=EF2_NONE) {
        elem=doc.createElement(tagEFFECTS);
        elem.setAttribute(attrEffect, static_cast<int>( effect ));
        elem.setAttribute(attrEffect2, static_cast<int>( effect2 ));
        elem.setAttribute( "speed", static_cast<int>(m_appearSpeed) );
        fragment.appendChild(elem);
    }
    if(appearStep!=0)
        fragment.appendChild(KPrObject::createValueElement(tagPRESNUM, appearStep, doc));
    if(angle!=0.0) {
        elem=doc.createElement(tagANGLE);
        elem.setAttribute(attrValue, angle);
        fragment.appendChild(elem);
    }
    if(effect3!=EF3_NONE || disappear) {
        elem=doc.createElement(tagDISAPPEAR);
        elem.setAttribute(attrEffect, static_cast<int>( effect3 ));
        elem.setAttribute(attrDoit, static_cast<int>( disappear ));
        elem.setAttribute( "speed", static_cast<int>(m_disappearSpeed) );
        elem.setAttribute(attrNum, disappearStep);
        fragment.appendChild(elem);
    }
    if(appearTimer!=1 || disappearTimer!=1) {
        elem=doc.createElement("TIMER");
        elem.setAttribute("appearTimer", appearTimer);
        elem.setAttribute("disappearTimer", disappearTimer);
        fragment.appendChild(elem);
    }
    if(appearSoundEffect || !a_fileName.isEmpty()) {
        elem=doc.createElement("APPEARSOUNDEFFECT");
        elem.setAttribute("appearSoundEffect", static_cast<int>(appearSoundEffect));
        elem.setAttribute("appearSoundFileName", a_fileName);
        fragment.appendChild(elem);
    }
    if(disappearSoundEffect || !d_fileName.isEmpty()) {
        elem=doc.createElement("DISAPPEARSOUNDEFFECT");
        elem.setAttribute("disappearSoundEffect", static_cast<int>(disappearSoundEffect));
        elem.setAttribute("disappearSoundFileName", d_fileName);
        fragment.appendChild(elem);
    }
    if( !m_objectName.isEmpty() ) {
        elem=doc.createElement("OBJECTNAME");
        elem.setAttribute("objectName", m_objectName);
        fragment.appendChild(elem);
    }
    if(protect) {
        elem=doc.createElement("PROTECT");
        elem.setAttribute("state" , protect);
        fragment.appendChild(elem);
    }
    if(keepRatio) {
        elem=doc.createElement("RATIO");
        elem.setAttribute("ratio" , keepRatio);
        fragment.appendChild(elem);
    }

    return fragment;
}


void KPrObject::saveOasisPosObject( KoXmlWriter &xmlWriter, int indexObj ) const
{
    xmlWriter.addAttribute( "draw:id", "object" + QString::number( indexObj ) );
    //save all into pt
    xmlWriter.addAttributePt( "svg:width", ext.width() );
    xmlWriter.addAttributePt( "svg:height", ext.height() );

    if ( qAbs( angle ) > 1E-6 )
    {
        double angInRad = -angle * M_PI / 180.0;
        QMatrix m( cos( angInRad ), -sin( angInRad ), sin( angInRad ), cos( angInRad ), 0, 0 );
        KoPoint center( ext.width() / 2, ext.height() / 2 );
        double rotX = 0.0;
        double rotY = 0.0;
        m.map( center.x(), center.y(), &rotX, &rotY );
        KoPoint rot( rotX, rotY );
        KoPoint trans( center - rot + orig );

        QString transX;
        transX.setNum( trans.x(), 'g', DBL_DIG );
        QString transY;
        transY.setNum( trans.y(), 'g', DBL_DIG );
        QString str = QString( "rotate(%1) translate(%2pt %3pt)" ).arg( angInRad ).arg( transX ).arg( transY );
        xmlWriter.addAttribute( "draw:transform", str );
    }
    else
    {
        xmlWriter.addAttributePt( "svg:x", orig.x() );
        xmlWriter.addAttributePt( "svg:y", orig.y() );
    }
}

void KPrObject::saveOasisObjectProtectStyle( KoGenStyle &styleobjectauto ) const
{
    if ( protect )
    {
        styleobjectauto.addProperty( "draw:move-protect", "true" );
        styleobjectauto.addProperty( "draw:size-protect", "true" );
    }
}

QString KPrObject::getStyle( KPOasisSaveContext &sc ) const
{
    kDebug(33001) << "KPrObject::getStyle" << endl;
    KoGenStyle styleObjectAuto;
    KoGenStyles &mainStyles( sc.context.mainStyles() );
    if ( sc.onMaster )
    {
        styleObjectAuto = KoGenStyle( KPrDocument::STYLE_PRESENTATIONSTICKYOBJECT, "presentation" );
    }
    else
    {
        styleObjectAuto = KoGenStyle( KoGenStyle::STYLE_GRAPHICAUTO, "graphic" );
    }
    fillStyle( styleObjectAuto, mainStyles );
    if ( sc.onMaster )
    {
        return mainStyles.lookup( styleObjectAuto, "pr" );
    }
    return mainStyles.lookup( styleObjectAuto, "gr" );
}

void KPrObject::fillStyle( KoGenStyle& styleObjectAuto, KoGenStyles& /* mainStyles */ ) const
{
    kDebug(33001) << "KPrObject::fillStyle" << endl;
    saveOasisObjectProtectStyle( styleObjectAuto );
    saveOasisShadowElement( styleObjectAuto );
}

bool KPrObject::saveOasisObjectAttributes( KPOasisSaveContext &/* sc */ ) const
{
    kDebug()<<"bool saveOasisObjectAttributes not implemented";
    return true;
}

bool KPrObject::hasAnimation() const
{
    return effect != EF_NONE || appearTimer != 0 ||
           effect3 != EF3_NONE || disappear ||
           !a_fileName.isEmpty() || ! d_fileName.isEmpty() ||
           appearTimer != 1;
}

bool KPrObject::saveOasisObjectStyleShowAnimation( KoXmlWriter &animation, int objectId )
{
    if ( effect != EF_NONE || appearStep != 0 || !a_fileName.isEmpty() )
    {
        animation.startElement( "presentation:show-shape" );
        animation.addAttribute( "draw:shape-id", "object" + QString::number( objectId ) );
        animation.addAttribute( "koffice:order-id", appearStep );

        switch( effect )
        {
        case EF_NONE:
            // use appear instead none as oo understands this
            animation.addAttribute( "presentation:effect", "appear" );
            break;
        case EF_COME_RIGHT:
            animation.addAttribute( "presentation:effect", "move" );
            animation.addAttribute( "presentation:direction", "from-right" );
            break;
        case EF_COME_LEFT:
            animation.addAttribute( "presentation:effect", "move" );
            animation.addAttribute( "presentation:direction", "from-left" );
            break;
        case EF_COME_TOP:
            animation.addAttribute( "presentation:effect", "move" );
            animation.addAttribute( "presentation:direction", "from-top" );
            break;
        case EF_COME_BOTTOM:
            animation.addAttribute( "presentation:effect", "move" );
            animation.addAttribute( "presentation:direction", "from-bottom" );
            break;
        case EF_COME_RIGHT_TOP:
            animation.addAttribute( "presentation:effect", "move" );
            animation.addAttribute( "presentation:direction", "from-upper-right" );
            break;
        case EF_COME_RIGHT_BOTTOM:
            animation.addAttribute( "presentation:effect", "move" );
            animation.addAttribute( "presentation:direction", "from-lower-right" );
            break;
        case EF_COME_LEFT_TOP:
            animation.addAttribute( "presentation:effect", "move" );
            animation.addAttribute( "presentation:direction", "from-upper-left" );
            break;
        case EF_COME_LEFT_BOTTOM:
            animation.addAttribute( "presentation:effect", "move" );
            animation.addAttribute( "presentation:direction", "from-lower-left" );
            break;
        case EF_WIPE_LEFT:
            animation.addAttribute( "presentation:effect", "fade" );
            animation.addAttribute( "presentation:direction", "from-left" );
            break;
        case EF_WIPE_RIGHT:
            animation.addAttribute( "presentation:effect", "fade" );
            animation.addAttribute( "presentation:direction", "from-right" );
            break;
        case EF_WIPE_TOP:
            animation.addAttribute( "presentation:effect", "fade" );
            animation.addAttribute( "presentation:direction", "from-top" );
            break;
        case EF_WIPE_BOTTOM:
            animation.addAttribute( "presentation:effect", "fade" );
            animation.addAttribute( "presentation:direction", "from-bottom" );
            break;
        }

        if ( effect2 != EF2_NONE )
        {
            animation.addAttribute( "koffice:by-paragraph", "true" );
        }

        if ( m_appearSpeed == ES_SLOW )
        {
            animation.addAttribute( "presentation:speed", "slow" );
        }
        else if ( m_appearSpeed == ES_FAST )
        {
            animation.addAttribute( "presentation:speed", "fast" );
        }

        if ( appearTimer!=1 )
        {
            animation.addAttribute( "presentation:animation-delay", saveOasisTimer( appearTimer ) );
        }
        if( !a_fileName.isEmpty() )
        {
            //store sound into file ?
             //<presentation:sound xlink:href="../../usr/lib/openoffice/share/gallery/sounds/pluck.wav" xlink:type="simple" xlink:show="new" xlink:actuate="onRequest"/>
            animation.startElement( "presentation:sound" );
            animation.addAttribute( "xlink:href", a_fileName );
            animation.addAttribute( "xlink:type", "simple" );
            animation.addAttribute( "xlink:show", "new" );
            animation.addAttribute( "xlink:actuate", "onRequest" );
            animation.endElement();
        }
        animation.endElement();
    }
    return true;
}

bool KPrObject::saveOasisObjectStyleHideAnimation( KoXmlWriter &animation, int objectId )
{

    //FIXME oo doesn't support hide animation object
    if ( effect3 != EF3_NONE || disappearStep != 0 || !d_fileName.isEmpty())
    {
        animation.startElement( "presentation:hide-shape" );
        animation.addAttribute( "draw:shape-id", "object" + QString::number( objectId ) );
        animation.addAttribute( "koffice:order-id", disappearStep );
        switch( effect3 )
        {
        case EF3_NONE:
            // use hide instead none as oo understands this
            animation.addAttribute( "presentation:effect", "hide" );
            break;
        case EF3_GO_RIGHT:
            animation.addAttribute( "presentation:effect", "move" );
            animation.addAttribute( "presentation:direction", "from-right" );
            break;
        case EF3_GO_LEFT:
            animation.addAttribute( "presentation:effect", "move" );
            animation.addAttribute( "presentation:direction", "from-left" );
            break;
        case EF3_GO_TOP:
            animation.addAttribute( "presentation:effect", "move" );
            animation.addAttribute( "presentation:direction", "from-top" );
            break;
        case EF3_GO_BOTTOM:
            animation.addAttribute( "presentation:effect", "move" );
            animation.addAttribute( "presentation:direction", "from-bottom" );
            break;
        case EF3_GO_RIGHT_TOP:
            animation.addAttribute( "presentation:effect", "move" );
            animation.addAttribute( "presentation:direction", "from-upper-right" );
            break;
        case EF3_GO_RIGHT_BOTTOM:
            animation.addAttribute( "presentation:effect", "move" );
            animation.addAttribute( "presentation:direction", "from-lower-right" );
            break;
        case EF3_GO_LEFT_TOP:
            animation.addAttribute( "presentation:effect", "move" );
            animation.addAttribute( "presentation:direction", "from-upper-left" );
            break;
        case EF3_GO_LEFT_BOTTOM:
            animation.addAttribute( "presentation:effect", "move" );
            animation.addAttribute( "presentation:direction", "from-lower-left" );
            break;
        case EF3_WIPE_LEFT:
            animation.addAttribute( "presentation:effect", "fade" );
            animation.addAttribute( "presentation:direction", "from-left" );
            break;
        case EF3_WIPE_RIGHT:
            animation.addAttribute( "presentation:effect", "fade" );
            animation.addAttribute( "presentation:direction", "from-right" );
            break;
        case EF3_WIPE_TOP:
            animation.addAttribute( "presentation:effect", "fade" );
            animation.addAttribute( "presentation:direction", "from-top" );
            break;
        case EF3_WIPE_BOTTOM:
            animation.addAttribute( "presentation:effect", "fade" );
            animation.addAttribute( "presentation:direction", "from-bottom" );
            break;
        }

        if ( m_disappearSpeed == ES_SLOW )
        {
            animation.addAttribute( "presentation:speed", "slow" );
        }
        else if ( m_disappearSpeed == ES_FAST )
        {
            animation.addAttribute( "presentation:speed", "fast" );
        }

        if ( disappearTimer!=1 )
        {
            animation.addAttribute( "presentation:animation-delay", saveOasisTimer( disappearTimer ) );
        }
        if( !d_fileName.isEmpty() )
        {
            //store sound into file ?
             //<presentation:sound xlink:href="../../usr/lib/openoffice/share/gallery/sounds/pluck.wav" xlink:type="simple" xlink:show="new" xlink:actuate="onRequest"/>
            animation.startElement( "presentation:sound" );
            animation.addAttribute( "xlink:href", a_fileName );
            animation.addAttribute( "xlink:type", "simple" );
            animation.addAttribute( "xlink:show", "new" );
            animation.addAttribute( "xlink:actuate", "onRequest" );

            animation.endElement();
        }
        animation.endElement();
    }
    return true;
}

void KPrObject::loadOasis(const QDomElement &element, KoOasisContext & context, KPrLoadingInfo *info)
{
    if(element.hasAttributeNS( KoXmlNS::draw, "name" ))
       m_objectName = element.attributeNS( KoXmlNS::draw, "name", QString::null);
    orig.setX( KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "x", QString::null ) ) );
    orig.setY( KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "y", QString::null ) ) );
    ext.setWidth(KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "width", QString::null )) );
    ext.setHeight(KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "height", QString::null ) ) );
    //kDebug()<<" orig.x() :"<<orig.x() <<" orig.y() :"<<orig.y() <<"ext.width() :"<<ext.width()<<" ext.height(): "<<ext.height()<<endl;
    KoStyleStack &styleStack = context.styleStack();
    styleStack.setTypeProperties( "" ); //no type default type
    if ( element.hasAttributeNS( KoXmlNS::draw, "transform" ) )
    {
        // there is some more stuff in the spezification
        // TODO make it work for all cases
        QString transform = element.attributeNS( KoXmlNS::draw, "transform", QString::null );
        kDebug()<<" transform action :"<<transform<<endl;
        QRegExp rx( "rotate ?\\(([^)]+)\\) translate ?\\(([^ ]+) ([^)]+)\\)" );
        if ( rx.search( transform ) != - 1 && rx.numCaptures() == 3 )
        {
            bool ok = false;
            double angInRad = rx.cap( 1 ).toDouble( &ok );
            if( ok )
            {
                angle = -angInRad * 180.0 / M_PI;
            }
            else
            {
                angle = 0.0;
                angInRad = 0.0;
            }
            QMatrix m( cos( angInRad ), -sin( angInRad ), sin( angInRad ), cos( angInRad ), 0, 0 );
            KoPoint center( ext.width() / 2, ext.height() / 2 );
            double transX = 0.0;
            double transY = 0.0;
            m.map( center.x(), center.y(), &transX, &transY );
            KoPoint diff( transX, transY );
            KoPoint trans( KoUnit::parseValue( rx.cap( 2 ) ), KoUnit::parseValue( rx.cap( 3 ) ) );
            orig = trans - center + diff;
            kDebug(33001) << "trans = " << trans << ", center = " << center << ", diff = " << diff << ", orig = " << orig << endl;
        }
    }
    QDomElement *animation = 0L;
    lstAnimation *tmp = 0L;
    if( element.hasAttributeNS( KoXmlNS::draw, "id"))
    {
        tmp = info->animationShowById(element.attributeNS( KoXmlNS::draw, "id", QString::null) );
        if ( tmp )
            animation = tmp->element;
    }

    if( animation)
    {
        QString effectStr = animation->attributeNS( KoXmlNS::presentation, "effect", QString::null);
        QString dir = animation->attributeNS( KoXmlNS::presentation, "direction", QString::null);
        QString speed = animation->attributeNS( KoXmlNS::presentation, "speed", QString::null );
        appearStep = tmp->order;
        kDebug()<<" appear direction : "<<dir<<" effect :"<< effectStr <<" speed :"<<speed<<endl;

        if ( speed =="medium" )
        {
            m_appearSpeed = ES_MEDIUM;
        }
        else if ( speed=="slow" )
        {
            m_appearSpeed = ES_SLOW;
        }
        else if ( speed=="fast" )
        {
            m_appearSpeed = ES_FAST;
        }
        else
            kDebug()<<" speed argument is not defined :"<<speed<<endl;

        if ( animation->hasAttributeNS( KoXmlNS::presentation, "animation-delay" ) )
        {
            appearTimer = loadOasisTimer(animation->attributeNS( KoXmlNS::presentation, "animation-delay", QString::null ) );
        }
        if (effectStr=="fade")
        {
            if (dir=="from-right")
                effect = EF_WIPE_RIGHT;
            else if (dir=="from-left")
                effect = EF_WIPE_LEFT;
            else if (dir=="from-top")
                effect=  EF_WIPE_TOP;
            else if (dir=="from-bottom")
                effect = EF_WIPE_BOTTOM;
            else
                kDebug(33001)<<" dir not supported: " << dir << endl;
        }
        else if (effectStr=="move")
        {
            if (dir=="from-right")
                effect = EF_COME_RIGHT;
            else if (dir=="from-left")
                effect = EF_COME_LEFT;
            else if (dir=="from-top")
                effect = EF_COME_TOP;
            else if (dir=="from-bottom")
                effect = EF_COME_BOTTOM;
            else if (dir=="from-upper-right")
                effect = EF_COME_RIGHT_TOP;
            else if (dir=="from-lower-right")
                effect = EF_COME_RIGHT_BOTTOM;
            else if (dir=="from-upper-left")
                effect = EF_COME_LEFT_TOP;
            else if (dir=="from-lower-left")
                effect = EF_COME_LEFT_BOTTOM;
            else
                kDebug(33001) << " dir not supported:" << dir << endl;
        }
        else if ( effectStr == "appear" )
        {
            effect = EF_NONE;
        }
        else
        {
            kDebug(33001) << " appear effect not supported: " << effectStr << endl;
            effect = EF_NONE;
        }

        QString byParagaph = animation->attributeNS( KoXmlNS::koffice, "by-paragraph", QString::null );
        if ( byParagaph == "true" )
        {
            effect2 = EF2T_PARA;
        }

        QDomElement sound = KoDom::namedItemNS( *animation, KoXmlNS::presentation, "sound" );
        if ( !sound.isNull() )
        {
            kDebug()<<" object has sound effect \n";
            if ( sound.hasAttributeNS( KoXmlNS::xlink, "href" ) )
            {
                a_fileName =sound.attributeNS( KoXmlNS::xlink, "href", QString::null );
                appearSoundEffect = true;
            }
        }
    }

    animation = 0L;
    tmp = 0L;
    if( element.hasAttributeNS( KoXmlNS::draw, "id"))
    {
        tmp = info->animationHideById(element.attributeNS( KoXmlNS::draw, "id", QString::null) );
        if ( tmp )
            animation = tmp->element;
    }

    if( animation)
    {
        QString effectStr = animation->attributeNS( KoXmlNS::presentation, "effect", QString::null);
        QString dir = animation->attributeNS( KoXmlNS::presentation, "direction", QString::null);
        QString speed = animation->attributeNS( KoXmlNS::presentation, "speed", QString::null );
        kDebug()<<" disappear direction : "<<dir<<" effect :"<< effectStr <<" speed :"<<speed<<endl;
        disappearStep = tmp->order;

        if ( speed =="medium" )
        {
            m_disappearSpeed = ES_MEDIUM;
        }
        else if ( speed=="slow" )
        {
            m_disappearSpeed = ES_SLOW;
        }
        else if ( speed=="fast" )
        {
            m_disappearSpeed = ES_FAST;
        }
        else
            kDebug()<<" speed argument is not defined :"<<speed<<endl;

        if ( animation->hasAttributeNS( KoXmlNS::presentation, "animation-delay" ) )
        {
            disappearTimer = loadOasisTimer(animation->attributeNS( KoXmlNS::presentation, "animation-delay", QString::null ) );
        }
        if (effectStr=="fade")
        {
            if (dir=="from-right")
                effect3 = EF3_WIPE_RIGHT;
            else if (dir=="from-left")
                effect3 = EF3_WIPE_LEFT;
            else if (dir=="from-top")
                effect3 =  EF3_WIPE_TOP;
            else if (dir=="from-bottom")
                effect3 = EF3_WIPE_BOTTOM;
            else
                kDebug(33001) << " dir not supported: " << dir << endl;
        }
        else if (effectStr=="move")
        {
            if (dir=="from-right")
                effect3 = EF3_GO_RIGHT;
            else if (dir=="from-left")
                effect3 = EF3_GO_LEFT;
            else if (dir=="from-top")
                effect3 = EF3_GO_TOP;
            else if (dir=="from-bottom")
                effect3 = EF3_GO_BOTTOM;
            else if (dir=="from-upper-right")
                effect3 = EF3_GO_RIGHT_TOP;
            else if (dir=="from-lower-right")
                effect3 = EF3_GO_RIGHT_BOTTOM;
            else if (dir=="from-upper-left")
                effect3 = EF3_GO_LEFT_TOP;
            else if (dir=="from-lower-left")
                effect3 = EF3_GO_LEFT_BOTTOM;
            else
                kDebug(33001) << " dir not supported: " << dir << endl;
        }
        else if ( effectStr == "hide" )
        {
            effect3 = EF3_NONE;
        }
        else
        {
            kDebug(33001) << " hide effect not supported: " << effectStr << endl;
            effect3 = EF3_NONE;
        }

        disappear = true;

        QDomElement sound = KoDom::namedItemNS( *animation, KoXmlNS::presentation, "sound" );
        if ( !sound.isNull() )
        {
            kDebug()<<" object has sound effect \n";
            if ( sound.hasAttributeNS( KoXmlNS::xlink, "href" ) )
            {
                d_fileName =sound.attributeNS( KoXmlNS::xlink, "href", QString::null );
                disappearSoundEffect = true;
            }
        }
    }
    //shadow
#if 0 //move it to kptextobject
    if ( !element.hasAttribute( "type" ) ||
         ( element.hasAttribute( "type" ) && element.attribute( "type" ) == "4" ) )
    {
        kDebug()<<" text document !!!!!\n";
        if ( styleStack.hasAttributeNS( KoXmlNS::fo, "text-shadow" ) &&
             styleStack.attributeNS( KoXmlNS::fo, "text-shadow" ) != "none" )
        {
            QString distance = styleStack.attributeNS( KoXmlNS::fo, "text-shadow" );
            distance.truncate( distance.find( ' ' ) );
            shadowDistance = (int)KoUnit::parseValue( distance );
            shadowDirection = SD_RIGHT_BOTTOM;
            shadowColor= QColor("#a0a0a0" );
        }
    }
#endif

// draw:textarea-horizontal-align="center" draw:textarea-vertical-align="middle" draw:shadow="visible" draw:move-protect="true" draw:size-protect="true"
    //kpresenter doesn't have two attribute for protect move and protect size perhaps create two argument for 1.4
    styleStack.setTypeProperties( "graphic" );
    if ( styleStack.hasAttributeNS( KoXmlNS::draw, "move-protect" ) )
    {
        kDebug()<<" styleStack.attribute(draw:move-protect ) :"<<styleStack.attributeNS( KoXmlNS::draw, "move-protect" )<<endl;
        protect = ( styleStack.attributeNS( KoXmlNS::draw, "move-protect" ) == "true" );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::draw, "size-protect" ) )
    {
        kDebug()<<" styleStack.attribute(draw:size-protect ) :"<<styleStack.attributeNS( KoXmlNS::draw, "size-protect" )<<endl;
        protect = ( styleStack.attributeNS( KoXmlNS::draw, "size-protect" ) == "true" );
    }

    //not supported into kpresenter
    if ( styleStack.hasAttributeNS( KoXmlNS::draw, "textarea-vertical-align" ) )
    {
        kDebug()<<" styleStack.attribute(draw:textarea-vertical-align ) :"<<styleStack.attributeNS( KoXmlNS::draw, "textarea-vertical-align" )<<endl;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::draw, "textarea-horizontal-align") )
    {
        kDebug()<<" styleStack.attribute(draw:textarea-horizontal-align ) :"<<styleStack.attributeNS( KoXmlNS::draw, "textarea-horizontal-align" )<<endl;
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::draw, "shadow" ) &&
              styleStack.attributeNS( KoXmlNS::draw, "shadow") == "visible" )
    {
        // use the shadow attribute to indicate an object-shadow
        double x = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::draw, "shadow-offset-x" ) );
        double y = KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::draw, "shadow-offset-y" ) );
        kDebug()<<" shadow x : "<<x<<" shadow y :"<<y<<endl;
        if ( x < 0 && y < 0 )
        {
            shadowDirection = SD_LEFT_UP;
            shadowDistance = (int) fabs ( x );
        }
        else if ( x == 0 && y < 0 )
        {
            shadowDirection = SD_UP;
            shadowDistance = (int) fabs ( y );
        }
        else if ( x > 0 && y < 0 )
        {
            shadowDirection = SD_RIGHT_UP;
            shadowDistance = (int) fabs ( x );
        }
        else if ( x > 0 && y == 0 )
        {
            shadowDirection = SD_RIGHT;
            shadowDistance = (int) fabs ( x );
        }
        else if ( x > 0 && y > 0 )
        {
            shadowDirection = SD_RIGHT_BOTTOM;
            shadowDistance = (int) fabs ( x );
        }
        else if ( x == 0 && y > 0 )
        {
            shadowDirection = SD_BOTTOM;
            shadowDistance = (int) fabs ( y );
        }
        else if ( x < 0 && y > 0 )
        {
            shadowDirection = SD_LEFT_BOTTOM;
            shadowDistance = (int) fabs ( x );
        }
        else if ( x < 0 && y == 0 )
        {
            shadowDirection = SD_LEFT;
            shadowDistance = (int) fabs ( x );
        }
        if ( styleStack.hasAttributeNS( KoXmlNS::draw, "shadow-color" ) )
            shadowColor= QColor(styleStack.attributeNS( KoXmlNS::draw, "shadow-color" ) );
        kDebug()<<" shadow color : "<<shadowColor.name()<<endl;
    }
}

bool KPrObject::saveOasisObject( KPOasisSaveContext &sc ) const
{
    sc.xmlWriter.startElement( getOasisElementName() );
    sc.xmlWriter.addAttribute( sc.onMaster ? "presentation:style-name": "draw:style-name", getStyle( sc ) );
    saveOasisPosObject( sc.xmlWriter, sc.indexObj );
    if( !m_objectName.isEmpty())
        sc.xmlWriter.addAttribute( "draw:name", m_objectName );

    saveOasisObjectAttributes( sc );

    sc.xmlWriter.endElement();
    return true;
}

void KPrObject::saveOasisShadowElement( KoGenStyle &styleobjectauto ) const
{
    //FIXME default value
    if(shadowDistance!=0 || shadowDirection!=SD_RIGHT_BOTTOM || shadowColor!=Qt::gray) {
        styleobjectauto.addProperty( "draw:shadow", "visible" );
        switch( shadowDirection )
        {
        case SD_LEFT_UP:
            styleobjectauto.addPropertyPt( "draw:shadow-offset-x", ( -1.0 * shadowDistance )  );
            styleobjectauto.addPropertyPt( "draw:shadow-offset-y", ( -1.0 * shadowDistance ) );
            break;
        case SD_UP:
            styleobjectauto.addPropertyPt( "draw:shadow-offset-x", 0.0 );
            styleobjectauto.addPropertyPt( "draw:shadow-offset-y", ( -1.0 * shadowDistance ) );
            break;
        case SD_RIGHT_UP:
            styleobjectauto.addPropertyPt( "draw:shadow-offset-x", shadowDistance );
            styleobjectauto.addPropertyPt( "draw:shadow-offset-y", ( -1.0 * shadowDistance ) );
            break;
        case SD_RIGHT:
            styleobjectauto.addPropertyPt( "draw:shadow-offset-x", shadowDistance );
            styleobjectauto.addPropertyPt( "draw:shadow-offset-y", 0.0 );
            break;
        case SD_RIGHT_BOTTOM:
            styleobjectauto.addPropertyPt( "draw:shadow-offset-x",shadowDistance  );
            styleobjectauto.addPropertyPt( "draw:shadow-offset-y", shadowDistance );
            break;
        case SD_BOTTOM:
            styleobjectauto.addPropertyPt( "draw:shadow-offset-x", 0.0 );
            styleobjectauto.addPropertyPt( "draw:shadow-offset-y",shadowDistance  );
            break;
        case SD_LEFT_BOTTOM:
            styleobjectauto.addPropertyPt( "draw:shadow-offset-x", ( -1.0*shadowDistance ) );
            styleobjectauto.addPropertyPt( "draw:shadow-offset-y", shadowDistance );
            break;
        case SD_LEFT:
            styleobjectauto.addPropertyPt( "draw:shadow-offset-x", ( -1.0 * shadowDistance ) );
            styleobjectauto.addPropertyPt( "draw:shadow-offset-y",  0.0 );
            break;
        }
        styleobjectauto.addProperty( "draw:shadow-color", shadowColor.name() );
    }
}

double KPrObject::load(const QDomElement &element) {

    double offset=-1.0;
    QDomElement e=element.namedItem(tagORIG).toElement();
    if(!e.isNull()) {
        if(e.hasAttribute(attrX))
            orig.setX(e.attribute(attrX).toDouble());
        if(e.hasAttribute(attrY))
        {
            offset=e.attribute(attrY).toDouble();
            orig.setY(0);
        }
    }
    e=element.namedItem(tagSIZE).toElement();
    if(!e.isNull()) {
        if(e.hasAttribute(attrWidth))
            ext.setWidth(e.attribute(attrWidth).toDouble());
        if(e.hasAttribute(attrHeight))
            ext.setHeight(e.attribute(attrHeight).toDouble());
    }
    e=element.namedItem(tagSHADOW).toElement();
    if(!e.isNull()) {
        if(e.hasAttribute(attrDistance))
            shadowDistance=e.attribute(attrDistance).toInt();
        if(e.hasAttribute(attrDirection))
            shadowDirection=static_cast<ShadowDirection>(e.attribute(attrDirection).toInt());
        shadowColor=retrieveColor(e);
    }
    else {
        shadowDistance=0;
        shadowDirection=SD_RIGHT_BOTTOM;
        shadowColor=Qt::gray;
    }
    e=element.namedItem(tagEFFECTS).toElement();
    if(!e.isNull()) {
        if(e.hasAttribute(attrEffect))
            effect=static_cast<Effect>(e.attribute(attrEffect).toInt());
        if(e.hasAttribute(attrEffect2))
            effect2=static_cast<Effect2>(e.attribute(attrEffect2).toInt());
        if(e.hasAttribute("speed"))
        {
            m_appearSpeed=static_cast<EffectSpeed>(e.attribute("speed").toInt());
            // this is a safty net as we had once speeds up to ten
            m_appearSpeed = m_appearSpeed > ES_FAST ? ES_FAST : m_appearSpeed;
        }
    }
    else {
        effect=EF_NONE;
        effect2=EF2_NONE;
    }
    e=element.namedItem(tagANGLE).toElement();
    if(!e.isNull()) {
        if(e.hasAttribute(attrValue))
            angle=e.attribute(attrValue).toFloat();
    }
    else
        angle=0.0;
    e=element.namedItem(tagPRESNUM).toElement();
    if(!e.isNull()) {
        if(e.hasAttribute(attrValue))
            appearStep=e.attribute(attrValue).toInt();
    }
    else
        appearStep=0;
    e=element.namedItem(tagDISAPPEAR).toElement();
    if(!e.isNull()) {
        if(e.hasAttribute(attrEffect))
            effect3=static_cast<Effect3>(e.attribute(attrEffect).toInt());
        if(e.hasAttribute(attrDoit))
            disappear=static_cast<bool>(e.attribute(attrDoit).toInt());
        if(e.hasAttribute(attrNum))
            disappearStep=e.attribute(attrNum).toInt();
        if(e.hasAttribute("speed"))
        {
            m_disappearSpeed=static_cast<EffectSpeed>(e.attribute("speed").toInt());
            // this is a safty net as we had once speeds up to ten
            m_disappearSpeed = m_appearSpeed > ES_FAST ? ES_FAST : m_appearSpeed;
        }
    }
    else {
        effect3=EF3_NONE;
        disappear=false;
        disappearStep=1;
    }
    e=element.namedItem("TIMER").toElement();
    if(!e.isNull()) {
        if(e.hasAttribute("appearTimer"))
            appearTimer = e.attribute("appearTimer").toInt();
        if(e.hasAttribute("disappearTimer"))
            disappearTimer = e.attribute("disappearTimer").toInt();
    }
    else {
        appearTimer = 1;
        disappearTimer = 1;
    }
    e=element.namedItem("APPEARSOUNDEFFECT").toElement();
    if(!e.isNull()) {
        if(e.hasAttribute("appearSoundEffect"))
            appearSoundEffect = static_cast<bool>(e.attribute("appearSoundEffect").toInt());
        if(e.hasAttribute("appearSoundFileName"))
            a_fileName = e.attribute("appearSoundFileName");
    }
    else {
        appearSoundEffect = false;
        a_fileName = QString::null;
    }
    e=element.namedItem("DISAPPEARSOUNDEFFECT").toElement();
    if(!e.isNull()) {
        if(e.hasAttribute("disappearSoundEffect"))
            disappearSoundEffect = static_cast<bool>(e.attribute("disappearSoundEffect").toInt());
        if(e.hasAttribute("disappearSoundFileName"))
            d_fileName = e.attribute("disappearSoundFileName");
    }
    else {
        disappearSoundEffect = false;
        d_fileName = QString::null;
    }
    e=element.namedItem("OBJECTNAME").toElement();
    if(!e.isNull()) {
        if(e.hasAttribute("objectName"))
            m_objectName = e.attribute("objectName");
    }
    else {
        m_objectName = QString::null;
    }
    e=element.namedItem("PROTECT").toElement();
    if (!e.isNull())
    {
        if(e.hasAttribute("state"))
            protect=static_cast<bool>(e.attribute("state").toInt());
    }

    e=element.namedItem("RATIO").toElement();
    if (!e.isNull())
    {
        if(e.hasAttribute("ratio"))
            keepRatio=static_cast<bool>(e.attribute("ratio").toInt());
    }

    return offset;
}

KoSize KPrObject::getRealSize() const {
    KoSize size = ext;

    if ( angle != 0.0 ) {
      float angInRad = angle * M_PI / 180;
      size.setWidth( ext.width() * fabs( cos( angInRad ) ) + ext.height() * fabs( sin( angInRad ) ) );
      size.setHeight( ext.width() * fabs( sin( angInRad ) ) + ext.height() * fabs( cos( angInRad ) ) );
    }

    return size;
}

KoPoint KPrObject::getRealOrig() const {
    KoPoint origin = orig;

    if ( angle != 0.0 ) {
        KoSize dist( ( getRealSize() - ext ) / 2 );
        origin.setX( orig.x() - dist.width() );
        origin.setY( orig.y() - dist.height() );
    }

    return origin;
}

KoRect KPrObject::getRealRect() const {
    return KoRect( getRealOrig(), getRealSize() );
}

KoRect KPrObject::getRepaintRect() const
{
    KoRect rect( getRealOrig(), getRealSize() );

    if ( shadowDirection == SD_LEFT ||
         shadowDirection == SD_LEFT_UP ||
         shadowDirection == SD_LEFT_BOTTOM )
    {
        rect.setLeft( rect.left() - shadowDistance );
    }
    if ( shadowDirection == SD_UP ||
         shadowDirection == SD_LEFT_UP ||
         shadowDirection == SD_RIGHT_UP )
    {
        rect.setTop( rect.top() - shadowDistance );
    }
    if ( shadowDirection == SD_RIGHT ||
         shadowDirection == SD_RIGHT_UP ||
         shadowDirection == SD_RIGHT_BOTTOM )
    {
        rect.setRight( rect.right() + shadowDistance );
    }
    if ( shadowDirection == SD_BOTTOM ||
         shadowDirection == SD_LEFT_BOTTOM ||
         shadowDirection == SD_RIGHT_BOTTOM )
    {
        rect.setBottom( rect.bottom() + shadowDistance );
    }

    if ( angle != 0.0 )
    {
        double _dx = rect.x() - 1.0;
        double _dy = rect.y() - 1.0;
        double _dw = rect.width() + 2.0;
        double _dh = rect.height() + 2.0;
        rect.setRect(  _dx, _dy, _dw, _dh );
    }

    return rect;
}

void KPrObject::flip( bool /*horizontal*/ ) {
    // flip the angle
    if ( angle ) {
        angle = 360.0 - angle;
    }
}

KoRect KPrObject::rotateRectObject() const
{
    KoRect br = KoRect( 0,0, ext.width(), ext.height() );
    double pw = br.width();
    double ph = br.height();
    KoRect rr = br;
    double yPos = -rr.y();
    double xPos = -rr.x();
    rr.moveTopLeft( KoPoint( -rr.width() / 2.0, -rr.height() / 2.0 ) );
    QMatrix m;
    m.translate( pw / 2.0, ph / 2.0 );
    m.rotate( angle );
    m.translate( rr.left() + xPos, rr.top() + yPos );
    KoRect r = KoRect::fromQRect(m.mapRect( br.toQRect() )); // see above TODO
    r.moveBy( orig.x() , orig.y() );
    return r;
}

void KPrObject::rotateObject(QPainter *paint,KoTextZoomHandler *_zoomHandler)
{
    KoRect rr = KoRect( 0, 0, ext.width(), ext.height() );
    rr.moveTopLeft( KoPoint( -ext.width() / 2.0, -ext.height() / 2.0 ) );
    QMatrix m;
    m.translate( _zoomHandler->zoomItXOld(ext.width() / 2.0), _zoomHandler->zoomItYOld(ext.height() / 2.0 ));
    m.rotate( angle );
    m.translate( _zoomHandler->zoomItXOld(rr.left()), _zoomHandler->zoomItYOld(rr.top()) );

    paint->setMatrix( m, true );
}

bool KPrObject::contains( const KoPoint &point ) const
{
    return getRealRect().contains( point );
}

bool KPrObject::intersects( const KoRect &rect ) const
{
    return getRealRect().intersects( rect );
}

QCursor KPrObject::getCursor( const KoPoint &_point, ModifyType &_modType,
                             KPrDocument *doc ) const
{
    KoTextZoomHandler * zh = doc->zoomHandler();
    int px = zh->zoomItXOld(_point.x());
    int py = zh->zoomItYOld(_point.y());

    QRect rect = zh->zoomRectOld( getRealRect() );
    int ox = rect.left();
    int oy = rect.top();
    int ow = rect.width();
    int oh = rect.height();

    bool headerFooter=doc->isHeaderFooter(this);

    int sz = 4;
    if ( px >= ox && py >= oy && px <= ox + qMin( ow / 3, sz ) && py <= oy + qMin( oh / 3, sz ) )
    {
        _modType = MT_RESIZE_LU;
        if ( protect)
            return Qt::ForbiddenCursor;
        return Qt::SizeFDiagCursor;
    }

    if ( px >= ox && py >= oy + oh / 2 - qMin( oh / 6, sz / 2 )
         && px <= ox + qMin( ow / 3, sz)
         && py <= oy + oh / 2 + qMin( oh / 6, sz / 2 ) )
    {
        _modType = MT_RESIZE_LF;
        if ( protect)
            return Qt::ForbiddenCursor;
        return Qt::SizeHorCursor;
    }

    if ( px >= ox && py >= oy + oh - qMin( oh / 3, sz ) && px <= ox + qMin( ow / 3, sz ) && py <= oy + oh )
    {
        _modType = MT_RESIZE_LD;
        if ( protect)
            return Qt::ForbiddenCursor;
        return Qt::SizeBDiagCursor;
    }

    if ( px >= ox + ow / 2 - qMin( ow / 6, sz / 2 ) && py >= oy
         && px <= ox + ow / 2 + qMin( ow / 6, sz / 2 )
         && py <= oy + qMin( oh / 3, sz ) )
    {
        _modType = MT_RESIZE_UP;
        if ( protect)
            return Qt::ForbiddenCursor;
        return Qt::SizeHorCursor;
    }

    if ( px >= ox + ow / 2 - qMin( ow / 6, sz / 2 ) && py >= oy + oh - qMin( oh / 3, sz )
         && px <= ox + ow / 2 + qMin( ow / 6, sz / 2 ) && py <= oy + oh )
    {
        _modType = MT_RESIZE_DN;
        if ( protect)
            return Qt::ForbiddenCursor;
        return Qt::SizeHorCursor;
    }

    if ( px >= ox + ow - qMin( ow / 3, sz ) && py >= oy && px <= ox + ow && py <= oy + qMin( oh / 3, sz) )
    {
        _modType = MT_RESIZE_RU;
        if ( protect)
            return Qt::ForbiddenCursor;
        return Qt::SizeBDiagCursor;
    }

    if ( px >= ox + ow - qMin( ow / 3, sz ) && py >= oy + oh / 2 - qMin( oh / 6, sz / 2 )
         && px <= ox + ow && py <= oy + oh / 2 + qMin( oh / 6, sz / 2) )
    {
        _modType = MT_RESIZE_RT;
        if ( protect)
            return Qt::ForbiddenCursor;
        return Qt::SizeHorCursor;
    }

    if ( px >= ox + ow - qMin( ow / 3, sz ) && py >= oy + oh - qMin( oh / 3, sz)
         && px <= ox + ow && py <= oy + oh )
    {
        _modType = MT_RESIZE_RD;
        if ( protect)
            return Qt::ForbiddenCursor;
        return Qt::SizeFDiagCursor;
    }

    //header footer can't move
    if(!headerFooter)
        _modType = MT_MOVE;
    if ( protect )
        return Qt::ForbiddenCursor;
    else
        return Qt::SizeAllCursor;
}

void KPrObject::getShadowCoords( double& _x, double& _y ) const
{
    double sx = 0, sy = 0;

    switch ( shadowDirection )
    {
    case SD_LEFT_UP:
    {
        sx = _x - shadowDistance;
        sy = _y - shadowDistance;
    } break;
    case SD_UP:
    {
        sx = _x;
        sy = _y - shadowDistance;
    } break;
    case SD_RIGHT_UP:
    {
        sx = _x + shadowDistance;
        sy = _y - shadowDistance;
    } break;
    case SD_RIGHT:
    {
        sx = _x + shadowDistance;
        sy = _y;
    } break;
    case SD_RIGHT_BOTTOM:
    {
        sx = _x + shadowDistance;
        sy = _y + shadowDistance;
    } break;
    case SD_BOTTOM:
    {
        sx = _x;
        sy = _y + shadowDistance;
    } break;
    case SD_LEFT_BOTTOM:
    {
        sx = _x - shadowDistance;
        sy = _y + shadowDistance;
    } break;
    case SD_LEFT:
    {
        sx = _x - shadowDistance;
        sy = _y;
    } break;
    }

    _x = sx; _y = sy;
}

void KPrObject::paintSelection( QPainter *_painter, KoTextZoomHandler *_zoomHandler, SelectionMode mode )
{
    if ( !selected || mode == SM_NONE )
        return;

    _painter->save();
    _painter->translate( _zoomHandler->zoomItXOld(orig.x()), _zoomHandler->zoomItYOld(orig.y()) );
    _painter->setPen( QPen( Qt::black, 1, Qt::SolidLine ) );
    _painter->setBrush( kapp->palette().color( QPalette::Active, QColorGroup::Highlight ) );

    KoRect r = getRealRect();

    int x = _zoomHandler->zoomItXOld( r.left() - orig.x());
    int y = _zoomHandler->zoomItYOld( r.top() - orig.y());
    int zX6 = /*_zoomHandler->zoomItXOld(*/ 6 ;
    int zY6 = /*_zoomHandler->zoomItYOld(*/ 6 ;
    int w = _zoomHandler->zoomItXOld(r.width()) - 6;
    int h = _zoomHandler->zoomItYOld(r.height()) - 6;

    if ( mode == SM_MOVERESIZE ) {
        _painter->drawRect( x, y,  zX6, zY6 );
        _painter->drawRect( x, y + h / 2, zX6, zY6 );
        _painter->drawRect( x, y + h, zX6, zY6 );
        _painter->drawRect( x + w, y, zX6, zY6 );
        _painter->drawRect( x + w, y + h / 2, zX6, zY6 );
        _painter->drawRect( x + w, y + h, zX6, zY6 );
        _painter->drawRect( x + w / 2, y,zX6, zY6 );
        _painter->drawRect( x + w / 2, y + h, zX6, zY6 );
    }
    else if ( mode == SM_PROTECT) {
        _painter->drawRect( x, y,  zX6, zY6 );
        _painter->drawRect( x, y + h / 2, zX6, zY6 );
        _painter->drawRect( x, y + h, zX6, zY6 );
        _painter->drawRect( x + w, y, zX6, zY6 );
        _painter->drawRect( x + w, y + h / 2, zX6, zY6 );
        _painter->drawRect( x + w, y + h, zX6, zY6 );
        _painter->drawRect( x + w / 2, y,zX6, zY6 );
        _painter->drawRect( x + w / 2, y + h, zX6, zY6 );

        x= x + 1;
        y= y + 1;
        zX6=zX6-2;
        zY6=zY6-2;

        QBrush brush=kapp->palette().color( QPalette::Active,QColorGroup::Base );
        _painter->fillRect( x, y,  zX6, zY6, brush );
        _painter->fillRect( x, y + h / 2, zX6, zY6, brush);
        _painter->fillRect( x, y + h, zX6, zY6, brush );
        _painter->fillRect( x + w, y, zX6, zY6, brush );
        _painter->fillRect( x + w, y + h / 2, zX6, zY6, brush );
        _painter->fillRect( x + w  , y + h , zX6 , zY6 , brush );
        _painter->fillRect( x + w / 2 , y ,zX6 , zY6 , brush );
        _painter->fillRect( x + w / 2, y + h , zX6 , zY6 , brush );
    }
    else if ( mode == SM_ROTATE ) {
        _painter->drawEllipse( x, y,  zX6, zY6 );
        _painter->drawEllipse( x, y + h, zX6, zY6 );
        _painter->drawEllipse( x + w, y, zX6, zY6 );
        _painter->drawEllipse( x + w, y + h, zX6, zY6 );
    }

    _painter->restore();
}

void KPrObject::doDelete()
{
    if ( cmds == 0 && !inObjList )
        delete this;
}

KPrObjectAdaptor* KPrObject::dbusObject()
{
    return dbus;
}

void KPrObject::setupClipRegion( QPainter *painter, const QRegion &clipRegion )
{
#warning "kde4: port it"
#if 0
    QRegion region = painter->clipRegion( QPainter::CoordPainter );
    if ( region.isEmpty() )
        region = clipRegion;
    else
        region.unite( clipRegion );

    painter->setClipRegion( region, QPainter::CoordPainter );
#endif
}

QDomElement KPrObject::createValueElement(const QString &tag, int value, QDomDocument &doc) {
    QDomElement elem=doc.createElement(tag);
    elem.setAttribute(attrValue, value);
    return elem;
}

QDomElement KPrObject::createGradientElement(const QString &tag, const QColor &c1, const QColor &c2,
                                            int type, bool unbalanced, int xfactor,
                                            int yfactor, QDomDocument &doc) {
    QDomElement elem=doc.createElement(tag);
    elem.setAttribute(attrC1, c1.name());
    elem.setAttribute(attrC2, c2.name());
    elem.setAttribute(attrType, type);
    elem.setAttribute(attrUnbalanced, (uint)unbalanced);
    elem.setAttribute(attrXFactor, xfactor);
    elem.setAttribute(attrYFactor, yfactor);
    return elem;
}

QDomElement KPrObject::createPenElement(const QString &tag, const KoPen &pen, QDomDocument &doc) {

    QDomElement elem=doc.createElement(tag);
    elem.setAttribute(attrColor, pen.color().name());
    elem.setAttribute(attrWidth, pen.pointWidth());
    elem.setAttribute(attrStyle, static_cast<int>(pen.style()));
    return elem;
}

KoPen KPrObject::toPen(const QDomElement &element) const {

    KoPen pen;
    pen.setColor(retrieveColor(element));
    if(element.hasAttribute(attrStyle))
        pen.setStyle(static_cast<Qt::PenStyle>(element.attribute(attrStyle).toInt()));
    if(element.hasAttribute(attrWidth))
        pen.setPointWidth(element.attribute(attrWidth).toDouble());
    return pen;
}

QDomElement KPrObject::createBrushElement(const QString &tag, const QBrush &brush, QDomDocument &doc) {

    QDomElement elem=doc.createElement(tag);
    elem.setAttribute(attrColor, brush.color().name());
    elem.setAttribute(attrStyle, static_cast<int>(brush.style()));
    return elem;
}

QBrush KPrObject::toBrush(const QDomElement &element) const {

    QBrush brush;
    brush.setColor(retrieveColor(element));
    if(element.hasAttribute(attrStyle))
        brush.setStyle(static_cast<Qt::BrushStyle>(element.attribute(attrStyle).toInt()));
    return brush;
}

QColor KPrObject::retrieveColor(const QDomElement &element, const QString &cattr,
                               const QString &rattr, const QString &gattr, const QString &battr) const {
    QColor ret;
    if(element.hasAttribute(cattr))
        ret.setNamedColor(element.attribute(cattr));
    else {
        int red=0, green=0, blue=0;
        if(element.hasAttribute(rattr))
            red=element.attribute(rattr).toInt();
        if(element.hasAttribute(gattr))
            green=element.attribute(gattr).toInt();
        if(element.hasAttribute(battr))
            blue=element.attribute(battr).toInt();
        ret.setRgb(red, green, blue);
    }
    return ret;
}

void KPrObject::draw( QPainter *_painter, KoTextZoomHandler*_zoomHandler,
                     int /*pageNum*/, SelectionMode selectionMode, bool drawContour )
{
    if ( selectionMode != SM_NONE && !drawContour )
        paintSelection( _painter, _zoomHandler, selectionMode );
}

KoPen KPrObject::getPen() const
{
    // Return the default pen
    return KoPen();
}

void KPrObject::getRealSizeAndOrigFromPoints( KoPointArray &points, float angle,
                                            KoSize &size, KoPoint &orig )
{
    if ( angle == 0 )
        return;

    float angInRad = angle * M_PI / 180;
    float sinus = sin( angInRad );
    float cosinus = cos( angInRad );

    float mid_x = size.width() / 2;
    float mid_y = size.height() / 2;

    float min_x = 0;
    float max_x = 0;
    float min_y = 0;
    float max_y = 0;
    KoPointArray::ConstIterator it;
    for ( it = points.begin(); it != points.end(); ++it ) {
        KoPoint cord( mid_x - (*it).x(), (*it).y() - mid_y );
        float tmp_x = cord.x() * cosinus + cord.y() * sinus;
        float tmp_y = - cord.x() * sinus + cord.y() * cosinus;

        if ( tmp_x < min_x ) {
            min_x = tmp_x;
        }
        else if ( tmp_x > max_x ) {
            max_x = tmp_x;
        }

        if ( tmp_y < min_y ) {
            min_y = tmp_y;
        }
        else if ( tmp_y > max_y ) {
            max_y = tmp_y;
        }
    }

    size.setWidth( max_x - min_x );
    size.setHeight( max_y - min_y );

    orig.setX( orig.x() + mid_x - max_x );
    orig.setY( orig.y() + mid_y + min_y );
}

void KPrObject::addSelfToGuides(Q3ValueList<double> &horizontalPos, Q3ValueList<double> &verticalPos )
{
    KoRect bounds = getRealRect();

    horizontalPos.append( bounds.top() );
    verticalPos.append( bounds.left() );
    horizontalPos.append( bounds.bottom() );
    verticalPos.append( bounds.right() );
}

KPrShadowObject::KPrShadowObject()
    : KPrObject()
{
}

KPrShadowObject::KPrShadowObject( const KoPen &_pen )
    : KPrObject(), pen( _pen )
{
}

KPrShadowObject &KPrShadowObject::operator=( const KPrShadowObject & )
{
    return *this;
}

QDomDocumentFragment KPrShadowObject::save( QDomDocument& doc,double offset )
{
    QDomDocumentFragment fragment=KPrObject::save(doc, offset);

    if(pen!=defaultPen())
        fragment.appendChild(KPrObject::createPenElement(tagPEN, pen, doc));
    return fragment;
}

void KPrShadowObject::saveOasisStrokeElement( KoGenStyles& mainStyles, KoGenStyle &styleobjectauto ) const
{
    if ( pen!=defaultPen() )
    {
        switch(  pen.style() )
        {
        case Qt::NoPen:
            styleobjectauto.addProperty( "draw:stroke" , "none" );
            break;
        case Qt::SolidLine:
            styleobjectauto.addProperty( "draw:stroke" , "solid" );
            break;
        case Qt::DashLine:
        case Qt::DotLine:
        case Qt::DashDotLine:
        case Qt::DashDotDotLine:
            styleobjectauto.addProperty( "draw:stroke" , "dash" );
            //TODO FIXME
            styleobjectauto.addProperty( "draw:stroke-dash", saveOasisStrokeStyle( mainStyles ) );
            break;
        default:
            break;
        }
        styleobjectauto.addProperty( "svg:stroke-color", pen.color().name() );
        styleobjectauto.addPropertyPt( "svg:stroke-width", pen.pointWidth() );
    }
}

QString KPrShadowObject::saveOasisStrokeStyle( KoGenStyles& mainStyles ) const
{
    KoGenStyle stroke( KPrDocument::STYLE_STROKE /*, "graphic"*/ /*no name*/ );
    switch( pen.style() )
    {
    case Qt::NoPen:
        //nothing
        break;
    case Qt::SolidLine:
        //nothing
        break;
    case Qt::DashLine: //value from ooimpress filter
        stroke.addAttribute( "draw:style", "rect" );
        stroke.addAttribute( "draw:dots1", "1" );
        stroke.addAttribute( "draw:dots2", "1" );
        stroke.addAttribute( "draw:dots1-length", "0.508cm" );
        stroke.addAttribute( "draw:dots2-length", "0.508cm" );
        stroke.addAttribute( "draw:distance", "0.508cm" );
        break;
    case Qt::DotLine:
        stroke.addAttribute( "draw:style", "rect" );
        stroke.addAttribute( "draw:dots1", "1" );
        stroke.addAttribute( "draw:distance", "0.257cm" );
        break;
    case Qt::DashDotLine:
        stroke.addAttribute( "draw:style", "rect" );
        stroke.addAttribute( "draw:dots1", "1" );
        stroke.addAttribute( "draw:dots2", "1" );
        stroke.addAttribute( "draw:dots1-length", "0.051cm" );
        stroke.addAttribute( "draw:dots2-length", "0.254cm" );
        stroke.addAttribute( "draw:distance", "0.127cm" );
        break;
    case Qt::DashDotDotLine:
        stroke.addAttribute( "draw:style", "rect" );
        stroke.addAttribute( "draw:dots1", "2" );
        stroke.addAttribute( "draw:dots2", "1" );
        stroke.addAttribute( "draw:dots2-length", "0.203cm" );
        stroke.addAttribute( "draw:distance", "0.203cm" );
        break;
    default:
        break;
    }
    return mainStyles.lookup( stroke, "stroke" );
    //    <draw:stroke-dash draw:name="Fine Dotted" draw:style="rect" draw:dots1="1" draw:distance="0.457cm"/>
}

bool KPrShadowObject::saveOasisDrawPoints( const KoPointArray &points, KPOasisSaveContext &sc )
{
    QString listOfPoint;
    int maxX=0;
    int maxY=0;
    KoPointArray::ConstIterator it( points.begin() );
    KoPointArray::ConstIterator end( points.end() );
    for ( ; it != end; ++it )
    {
        int tmpX = int( ( *it ).x() * 10000 );
        int tmpY = int( ( *it ).y() * 10000 );
        // no space allows before first element
        if ( !listOfPoint.isEmpty() )
            listOfPoint += QString( " %1,%2" ).arg( tmpX ).arg( tmpY );
        else
            listOfPoint = QString( "%1,%2" ).arg( tmpX ).arg( tmpY );
        maxX = qMax( maxX, tmpX );
        maxY = qMax( maxY, tmpY );
    }
    sc.xmlWriter.addAttribute("draw:points", listOfPoint );
    sc.xmlWriter.addAttribute("svg:viewBox", QString( "0 0 %1 %2" ).arg( maxX ).arg( maxY ) );
    return true;
}

bool KPrShadowObject::loadOasisDrawPoints( KoPointArray &points, const QDomElement &element,
                                          KoOasisContext & context, KPrLoadingInfo *info )
{
    Q_UNUSED( context );
    Q_UNUSED( info );
    QStringList ptList = QStringList::split(' ', element.attributeNS( KoXmlNS::draw, "points", QString::null));

    unsigned int index = 0;
    for (QStringList::Iterator it = ptList.begin(); it != ptList.end(); ++it)
    {
        points.putPoints( index++, 1, ( *it ).section( ',', 0, 0 ).toInt(), ( *it ).section( ',', 1, 1 ).toInt() );
    }
    loadOasisApplyViewBox( element, points );
    return true;
}


bool KPrShadowObject::loadOasisApplyViewBox( const QDomElement &element, KoPointArray &points )
{
    kDebug(33001) << "loadOasisApplayViewBox svg:viewBox = " << element.attributeNS( KoXmlNS::svg, "viewBox", QString::null ) << endl;
    QStringList viewBoxPoints = QStringList::split( ' ', element.attributeNS( KoXmlNS::svg, "viewBox", QString::null ) );

    KoRect viewBox;
    if ( viewBoxPoints.size() == 4 )
    {
        QStringList::Iterator it = viewBoxPoints.begin();
        //viewBox.setCoords( ( *it++ ).toInt(), ( *it++ ).toInt(), ( *it++ ).toInt(), ( *it ).toInt() );
        viewBox.setLeft( ( *it++ ).toInt() );
        viewBox.setTop( ( *it++ ).toInt() );
        viewBox.setRight( ( *it++ ).toInt() );
        viewBox.setBottom( ( *it ).toInt() );

        kDebug(33001) << "viewBox supplied = " << viewBox << endl;
    }
    else
    {
        //if no viewBox is found
        KoPointArray::ConstIterator it( points.begin() );
        bool first = true;
        for ( ; it != points.end(); ++it )
        {
            KoPoint p = (*it);
            if ( first )
            {
                viewBox.setCoords( p.x(), p.y(), p.x(), p.y() );
                first = false;
            }
            else
            {
                viewBox.setLeft( qMin( viewBox.left(), p.x() ) );
                viewBox.setRight( qMax( viewBox.right(), p.x() ) );
                viewBox.setTop( qMin( viewBox.top(), p.y() ) );
                viewBox.setBottom( qMax( viewBox.bottom(), p.y() ) );
            }
        }
        kDebug(33001) << "viewBox calculated = " << viewBox << endl;
    }

    if ( viewBox.width() != 0 && viewBox.height() != 0 )
    {
        KoPointArray::Iterator it( points.begin() );
        for ( ; it != points.end(); ++it )
        {
            KoPoint *p = it;
            p->setX( ( p->x() - viewBox.left() ) / viewBox.width() * ext.width() );
            p->setY( ( p->y() - viewBox.top() ) / viewBox.height() * ext.height() );
        }
    }
    else
    {
        kDebug(33001) << "problem in viewBox: " << viewBox << endl;
    }
    return true;
}


void KPrShadowObject::fillStyle( KoGenStyle& styleObjectAuto, KoGenStyles& mainStyles ) const
{
    kDebug(33001) << "KPrShadowObject::fillStyle" << endl;
    KPrObject::fillStyle( styleObjectAuto, mainStyles );
    saveOasisStrokeElement( mainStyles, styleObjectAuto );
}

void KPrShadowObject::loadOasis(const QDomElement &element, KoOasisContext & context, KPrLoadingInfo *info)
{
    //kDebug()<<"void KPrShadowObject::loadOasis(const QDomElement &element)**********************\n";
    KPrObject::loadOasis(element, context, info);
    KoStyleStack &styleStack = context.styleStack();
    styleStack.setTypeProperties( "graphic" );
    if ( styleStack.hasAttributeNS( KoXmlNS::draw, "stroke" ))
    {
        if ( styleStack.attributeNS( KoXmlNS::draw, "stroke" ) == "none" )
            pen.setStyle(Qt::NoPen );
        else if ( styleStack.attributeNS( KoXmlNS::draw, "stroke" ) == "solid" )
            pen.setStyle(Qt::SolidLine );
        else if ( styleStack.attributeNS( KoXmlNS::draw, "stroke" ) == "dash" )
        {
            QString style = styleStack.attributeNS( KoXmlNS::draw, "stroke-dash" );

            kDebug()<<" stroke style is  : "<<style<<endl;
            //type not defined by default
            //try to use style.
            QDomElement* draw = context.oasisStyles().drawStyles()[style];
            kDebug()<<" stroke have oasis style defined :"<<draw<<endl;
            if ( draw )
            {
                //FIXME
                if ( draw->attributeNS( KoXmlNS::draw, "style", QString::null )=="rect" )
                {
                    if ( draw->attributeNS( KoXmlNS::draw, "dots1", QString::null )=="1" &&
                         draw->attributeNS( KoXmlNS::draw, "dots2", QString::null )=="1" &&
                         draw->attributeNS( KoXmlNS::draw, "dots1-length", QString::null )=="0.508cm" &&
                         draw->attributeNS( KoXmlNS::draw, "dots2-length", QString::null )=="0.508cm" &&
                         draw->attributeNS( KoXmlNS::draw, "distance", QString::null )=="0.508cm" )
                        pen.setStyle( Qt::DashLine );
                    else if ( draw->attributeNS( KoXmlNS::draw, "dots1", QString::null )=="1" &&
                              draw->attributeNS( KoXmlNS::draw, "distance", QString::null )=="0.257cm" )
                        pen.setStyle(Qt::DotLine );
                    else if ( draw->attributeNS( KoXmlNS::draw, "dots1", QString::null )=="1" &&
                         draw->attributeNS( KoXmlNS::draw, "dots2", QString::null )=="1" &&
                         draw->attributeNS( KoXmlNS::draw, "dots1-length", QString::null )=="0.051cm" &&
                         draw->attributeNS( KoXmlNS::draw, "dots2-length", QString::null )=="0.254cm" &&
                         draw->attributeNS( KoXmlNS::draw, "distance", QString::null )=="0.127cm" )
                        pen.setStyle(Qt::DashDotLine );
                    else if ( draw->attributeNS( KoXmlNS::draw, "dots1", QString::null )=="2" &&
                         draw->attributeNS( KoXmlNS::draw, "dots2", QString::null )=="1" &&
                         draw->attributeNS( KoXmlNS::draw, "dots2-length", QString::null )=="0.203cm" &&
                         draw->attributeNS( KoXmlNS::draw, "distance", QString::null )=="0.203cm" )
                        pen.setStyle(Qt::DashDotDotLine );
                    else
                    {
                        kDebug()<<" stroke style undefined \n";
                        pen.setStyle(Qt::SolidLine );
                    }

                }
            }
        }
        //FIXME witdh pen style is not good :(
        if ( styleStack.hasAttributeNS( KoXmlNS::svg, "stroke-width" ) )
            pen.setPointWidth( KoUnit::parseValue( styleStack.attributeNS( KoXmlNS::svg, "stroke-width" ) ) );
        if ( styleStack.hasAttributeNS( KoXmlNS::svg, "stroke-color" ) )
            pen.setColor( styleStack.attributeNS( KoXmlNS::svg, "stroke-color" ) );
    }
    else
        pen = defaultPen();
    kDebug()<<"pen style :"<<pen<<endl;
}

double KPrShadowObject::load(const QDomElement &element)
{
    double offset=KPrObject::load(element);
    QDomElement e=element.namedItem(tagPEN).toElement();
    if(!e.isNull())
        setPen(KPrObject::toPen(e));
    else
        pen = defaultPen();
    return offset;
}

void KPrShadowObject::draw( QPainter *_painter, KoTextZoomHandler*_zoomHandler,
                           int pageNum, SelectionMode selectionMode, bool drawContour )
{
    double ox = orig.x();
    double oy = orig.y();
    _painter->save();

    // Draw the shadow if any
    if ( shadowDistance > 0 && !drawContour )
    {
        _painter->save();
        // tz TODO fix tmpPen usage
        KoPen tmpPen( pen );
        pen.setColor( shadowColor );
        QBrush brush;
        brush.setColor( shadowColor );

        double sx = ox;
        double sy = oy;
        getShadowCoords( sx, sy );

        _painter->translate( _zoomHandler->zoomItXOld( sx ), _zoomHandler->zoomItYOld( sy ) );

        if ( angle != 0 )
        {
            rotateObject( _painter, _zoomHandler );
        }

        paint( _painter, _zoomHandler, pageNum, true, drawContour );

        pen = tmpPen;
        _painter->restore();
    }

    _painter->translate( _zoomHandler->zoomItXOld(ox), _zoomHandler->zoomItYOld(oy) );

    if ( angle != 0 )
        rotateObject(_painter,_zoomHandler);
    paint( _painter, _zoomHandler, pageNum, false, drawContour );

    _painter->restore();

    KPrObject::draw( _painter, _zoomHandler, pageNum, selectionMode, drawContour );
}

KoPen KPrShadowObject::defaultPen() const
{
    return KoPen();
}

void KPrShadowObject::paint(QPainter* painter, KoTextZoomHandler* zoomHandler, int pageNum, bool drawingShadow, bool drawContour) {
    Q_UNUSED(painter);
    Q_UNUSED(zoomHandler);
    Q_UNUSED(pageNum);
    Q_UNUSED(drawingShadow);
    Q_UNUSED(drawContour);
}

KPr2DObject::KPr2DObject()
    : KPrShadowObject()
    , gradient( 0 )
    , m_redrawGradientPix( false )
{
}

KPr2DObject::KPr2DObject( const KoPen &_pen, const QBrush &_brush, FillType _fillType,
                        const QColor &_gColor1, const QColor &_gColor2, BCType _gType,
                        bool _unbalanced, int _xfactor, int _yfactor )
    : KPrShadowObject( _pen )
    , m_brush( _brush, _gColor1, _gColor2, _gType, _fillType, _unbalanced, _xfactor, _yfactor )
    , gradient( 0 )
    , m_redrawGradientPix( false )
{
    if ( getFillType() == FT_GRADIENT )
    {
        gradient = new KPrGradient( getGColor1(), getGColor2(),
                                   getGType(), getGUnbalanced(),
                                   getGXFactor(), getGYFactor() );
        m_redrawGradientPix = true;
    }
}

void KPr2DObject::setFillType( FillType _fillType )
{
    m_brush.setFillType( _fillType );

    if ( _fillType == FT_BRUSH && gradient )
    {
        delete gradient;
        gradient = 0;
    }
    if ( _fillType == FT_GRADIENT && !gradient )
    {
        gradient = new KPrGradient( getGColor1(), getGColor2(),
                                   getGType(), getGUnbalanced(),
                                   getGXFactor(), getGYFactor() );
        m_redrawGradientPix = true;
    }
}

QDomDocumentFragment KPr2DObject::save( QDomDocument& doc,double offset )
{
    QDomDocumentFragment fragment=KPrShadowObject::save(doc, offset);
    QBrush brush = getBrush();
    if ( brush != QBrush() )
        fragment.appendChild( KPrObject::createBrushElement( tagBRUSH, brush, doc ) );

    FillType fillType = getFillType();
    if ( fillType != FT_BRUSH )
        fragment.appendChild( KPrObject::createValueElement( tagFILLTYPE, static_cast<int>(fillType), doc ) );
    QColor gColor1 = getGColor1();
    QColor gColor2 = getGColor2();
    BCType gType = getGType();
    bool unbalanced = getGUnbalanced();
    int xfactor = getGXFactor();
    int yfactor = getGYFactor();
    if ( gColor1!=Qt::red || gColor2!=Qt::green || gType!=BCT_GHORZ || unbalanced || xfactor != 100 || yfactor != 100 )
        fragment.appendChild( KPrObject::createGradientElement( tagGRADIENT, gColor1, gColor2, static_cast<int>(gType),
                                                               unbalanced, xfactor, yfactor, doc));
    return fragment;
}

void KPr2DObject::fillStyle( KoGenStyle& styleObjectAuto, KoGenStyles& mainStyles ) const
{
    kDebug(33001) << "KPr2DObject::fillStyle" << endl;
    KPrShadowObject::fillStyle( styleObjectAuto, mainStyles );
    m_brush.saveOasisFillStyle( styleObjectAuto, mainStyles );
}


void KPr2DObject::loadOasis(const QDomElement &element, KoOasisContext & context, KPrLoadingInfo *info)
{
    kDebug()<<"void KPr2DObject::loadOasis(const QDomElement &element)\n";
    QBrush tmpBrush;

    KPrShadowObject::loadOasis(element, context, info);
    m_brush.loadOasisFillStyle( context, "graphic" );
    setFillType( m_brush.getFillType() );
}

double KPr2DObject::load(const QDomElement &element)
{
    double offset=KPrShadowObject::load(element);

    QDomElement e=element.namedItem(tagFILLTYPE).toElement();
    if(!e.isNull()) {
        if(e.hasAttribute(attrValue))
            setFillType(static_cast<FillType>(e.attribute(attrValue).toInt()));
    }
    else
        setFillType(FT_BRUSH);

    e=element.namedItem(tagBRUSH).toElement();
    if(!e.isNull())
        setBrush(KPrObject::toBrush(e));
    else
        setBrush( QBrush() );

    e=element.namedItem(tagGRADIENT).toElement();
    if(!e.isNull()) {
        setGColor1( retrieveColor( e, attrC1, "red1", "green1", "blue1" ) );
        setGColor2( retrieveColor( e, attrC2, "red2", "green2", "blue2" ) );
        if( e.hasAttribute( attrType ) )
            setGType( static_cast<BCType>( e.attribute( attrType ).toInt() ) );
        if( e.hasAttribute( attrUnbalanced ) )
            setGUnbalanced( static_cast<bool>( e.attribute( attrUnbalanced ).toInt() ) );
        if( e.hasAttribute( attrXFactor ) )
            setGXFactor( e.attribute( attrXFactor ).toInt() );
        if( e.hasAttribute( attrYFactor ) )
            setGYFactor( e.attribute( attrYFactor ).toInt() );
        if(gradient)
            gradient->setParameters(getGColor1(), getGColor2(), getGType(), getGUnbalanced(), getGXFactor(), getGYFactor() );
    }
    else {
        setGColor1( Qt::red );
        setGColor2( Qt::green );
        setGType( BCT_GHORZ );
        setGUnbalanced( false );
        setGXFactor( 100 );
        setGYFactor( 100 );
    }
    return offset;
}

void KPr2DObject::draw( QPainter *_painter, KoTextZoomHandler*_zoomHandler,
                           int pageNum, SelectionMode selectionMode, bool drawContour )
{
    double ox = orig.x();
    double oy = orig.y();
    _painter->save();

    // Draw the shadow if any
    if ( shadowDistance > 0 && !drawContour )
    {
        _painter->save();
        KoPen tmpPen( pen );
        pen.setColor( shadowColor );
        QBrush tmpBrush( m_brush.getBrush() );
        QBrush shadowBrush( tmpBrush );
        shadowBrush.setColor( shadowColor );
        m_brush.setBrush( shadowBrush );

        double sx = ox;
        double sy = oy;
        getShadowCoords( sx, sy );

        _painter->translate( _zoomHandler->zoomItXOld( sx ), _zoomHandler->zoomItYOld( sy ) );

        if ( angle != 0 )
        {
            rotateObject( _painter, _zoomHandler );
        }

        paint( _painter, _zoomHandler, pageNum, true, drawContour );

        pen = tmpPen;
        m_brush.setBrush( tmpBrush );
        _painter->restore();
    }

    _painter->translate( _zoomHandler->zoomItXOld(ox), _zoomHandler->zoomItYOld(oy) );

    if ( angle != 0 )
        rotateObject(_painter,_zoomHandler);
    paint( _painter, _zoomHandler, pageNum, false, drawContour );

    _painter->restore();

    KPrObject::draw( _painter, _zoomHandler, pageNum, selectionMode, drawContour );
}

void KPr2DObject::flip( bool horizontal ) {
    KPrObject::flip( horizontal );

    // flip the gradient
    if ( getFillType() == FT_GRADIENT ) {
        BCType gType = getGType();
        if ( gType == BCT_GDIAGONAL1 ) {
            setGType( BCT_GDIAGONAL2 );
        }
        else if ( gType == BCT_GDIAGONAL2 ) {
            setGType( BCT_GDIAGONAL1 );
        }
        if ( ( ! horizontal && gType == BCT_GDIAGONAL1 ) ||
             ( ! horizontal && gType == BCT_GDIAGONAL2 ) ||
             ( ! horizontal && gType == BCT_GHORZ ) ||
             ( horizontal && gType == BCT_GVERT ) ) {
            QColor gColorTemp;
            gColorTemp = getGColor1();
            setGColor1( getGColor2() );
            setGColor2( gColorTemp );
        }
        delete gradient;
        gradient = new KPrGradient( getGColor1(), getGColor2(), gType, getGUnbalanced(), getGXFactor(), getGYFactor() );
    }
}
