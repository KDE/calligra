// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "KPresenterObjectIface.h"

#include "kpobject.h"

#include <qpainter.h>
#include <qwmatrix.h>
#include <qpointarray.h>
#include <qregion.h>
#include <qdom.h>
#include <qbuffer.h>

#include <kapplication.h>
#include <kooasiscontext.h>


#include <stdlib.h>
#include <fstream>
#include <math.h>

#include <kozoomhandler.h>
#include <koRect.h>
#include <koSize.h>
#include <koPoint.h>
#include <kdebug.h>
#include "kpresenter_doc.h"
#include "kpresenter_utils.h"

const QString &KPObject::tagORIG=KGlobal::staticQString("ORIG");
const QString &KPObject::attrX=KGlobal::staticQString("x");
const QString &KPObject::attrY=KGlobal::staticQString("y");
const QString &KPObject::tagSIZE=KGlobal::staticQString("SIZE");
const QString &KPObject::attrWidth=KGlobal::staticQString("width");
const QString &KPObject::attrHeight=KGlobal::staticQString("height");
const QString &KPObject::tagSHADOW=KGlobal::staticQString("SHADOW");
const QString &KPObject::attrDistance=KGlobal::staticQString("distance");
const QString &KPObject::attrDirection=KGlobal::staticQString("direction");
const QString &KPObject::attrColor=KGlobal::staticQString("color");
const QString &KPObject::tagEFFECTS=KGlobal::staticQString("EFFECTS");
const QString &KPObject::attrEffect=KGlobal::staticQString("effect");
const QString &KPObject::attrEffect2=KGlobal::staticQString("effect2");
const QString &KPObject::tagPRESNUM=KGlobal::staticQString("PRESNUM");
const QString &KPObject::tagANGLE=KGlobal::staticQString("ANGLE");
const QString &KPObject::tagDISAPPEAR=KGlobal::staticQString("DISAPPEAR");
const QString &KPObject::attrDoit=KGlobal::staticQString("doit");
const QString &KPObject::attrNum=KGlobal::staticQString("num");
const QString &KPObject::tagFILLTYPE=KGlobal::staticQString("FILLTYPE");
const QString &KPObject::tagGRADIENT=KGlobal::staticQString("GRADIENT");
const QString &KPObject::tagPEN=KGlobal::staticQString("PEN");
const QString &KPObject::tagBRUSH=KGlobal::staticQString("BRUSH");
const QString &KPObject::attrValue=KGlobal::staticQString("value");
const QString &KPObject::attrC1=KGlobal::staticQString("color1");
const QString &KPObject::attrC2=KGlobal::staticQString("color2");
const QString &KPObject::attrType=KGlobal::staticQString("type");
const QString &KPObject::attrUnbalanced=KGlobal::staticQString("unbalanced");
const QString &KPObject::attrXFactor=KGlobal::staticQString("xfactor");
const QString &KPObject::attrYFactor=KGlobal::staticQString("yfactor");
const QString &KPObject::attrStyle=KGlobal::staticQString("style");

KPStartEndLine::KPStartEndLine( LineEnd _start, LineEnd _end )
    : lineBegin( _start ), lineEnd( _end )
{
}

void KPStartEndLine::save( QDomDocumentFragment &fragment, QDomDocument& doc )
{
    if (lineBegin!=L_NORMAL)
        fragment.appendChild(KPStartEndLine::createValueElement("LINEBEGIN", static_cast<int>(lineBegin), doc));
    if (lineEnd!=L_NORMAL)
        fragment.appendChild(KPStartEndLine::createValueElement("LINEEND", static_cast<int>(lineEnd), doc));
}

QDomElement KPStartEndLine::createValueElement(const QString &tag, int value, QDomDocument &doc)
{
    QDomElement elem=doc.createElement(tag);
    elem.setAttribute("value", value);
    return elem;
}


void KPStartEndLine::load( const QDomElement &element )
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

void KPStartEndLine::loadOasisMarkerElement( KoOasisContext & context, const QString & attr, LineEnd &_element )
{
    kdDebug()<<"void KPStartEndLine::loadOasisMarkerElement( KoOasisContext & context, const QString & attr, LineEnd &_element ) :"<<attr<<endl;

    KoStyleStack &styleStack = context.styleStack();
    styleStack.setTypeProperties( "graphic" );
    if ( styleStack.hasAttribute( attr ) )
    {
        QString style = styleStack.attribute( attr );
        //kdDebug()<<" marker style is  : "<<style<<endl;

        //type not defined by default
        //try to use style.
        QDomElement* draw = context.oasisStyles().drawStyles()[style];
        //kdDebug()<<" marker have oasis style defined :"<<draw<<endl;
        if ( draw )
        {
            if( draw->hasAttribute( "svg:d" ))
            {
                QString str = draw->attribute( "svg:d" );
                kdDebug()<<" svg type = "<<str<<endl;
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
                    kdDebug()<<" element not defined :"<<str<<endl;
                    _element = L_NORMAL;
                }
            }
        }
    }
}

void KPStartEndLine::saveOasisMarkerElement( KoGenStyles& mainStyles,  KoGenStyle &styleobjectauto ) const
{
    //FIXME
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

QString KPStartEndLine::saveOasisMarkerStyle( KoGenStyles &mainStyles, const LineEnd &_element ) const
{
    KoGenStyle marker( KPresenterDoc::STYLE_MARKER /*, "graphic"*/ /*no name*/ );
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

KPObject::KPObject()
    : orig(), ext(), shadowColor( Qt::gray ), sticky( FALSE )
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
    objectName = QString::null;
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
    sticky = false;
    protect = false;
    keepRatio = false;
    dcop = 0;
}

KPObject::~KPObject()
{
    delete dcop;
}

QDomDocumentFragment KPObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment=doc.createDocumentFragment();
    QDomElement elem=doc.createElement(tagORIG);
    elem.setAttribute(attrX, orig.x());
    elem.setAttribute(attrY, orig.y()+offset);
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
        fragment.appendChild(KPObject::createValueElement(tagPRESNUM, appearStep, doc));
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
    if( !objectName.isEmpty() ) {
        elem=doc.createElement("OBJECTNAME");
        elem.setAttribute("objectName", objectName);
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


void KPObject::saveOasisPosObject( KoXmlWriter &xmlWriter, int indexObj ) const
{
    xmlWriter.addAttribute( "draw:id", indexObj );
    //save all into pt
    xmlWriter.addAttributePt( "svg:x", orig.x() );
    xmlWriter.addAttributePt( "svg:y", orig.y() );
    xmlWriter.addAttributePt( "svg:width", ext.width() );
    xmlWriter.addAttributePt( "svg:height", ext.height() );
    //FIXME create style (protect object etc)

    if ( angle!=0.0 )
    {
        double value = -1 * ( ( double )angle* M_PI )/180.0;
        QString str=QString( "rotate (%1)" ).arg( value );
        xmlWriter.addAttribute( "draw:transform", str );

    }
}

void KPObject::saveOasisObjectProtectStyle( KoGenStyle &styleobjectauto ) const
{
    if ( protect )
    {
        styleobjectauto.addProperty( "draw:move-protect", "true" );
        styleobjectauto.addProperty( "draw:size-protect", "true" );
    }
}

bool KPObject::haveAnimation() const
{
    //kdDebug()<<" effect :"<<effect<<" effect3 :"<<effect3<<" a_fileName :"<<a_fileName<<" d_fileName :"<<d_fileName<<" appearTimer :"<<appearTimer<<" disappearTimer :"<<disappearTimer<<endl;
    if ( effect == EF_NONE && effect3==EF3_NONE && a_fileName.isEmpty() && d_fileName.isEmpty() && ( appearTimer==1 ) && ( disappearTimer==1 ))
        return false;
    else
        return true;
}

bool KPObject::saveOasisObjectStyleShowAnimation( KoXmlWriter &animation, int objectId )
{
    if ( effect != EF_NONE || !a_fileName.isEmpty() )
    {
        animation.startElement( "presentation:show-shape" );
        animation.addAttribute( "draw:shape-id", objectId );
        switch( effect )
        {
        case EF_NONE:
            animation.addAttribute( "presentation:effect", "none" );
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

bool KPObject::saveOasisObjectStyleHideAnimation( KoXmlWriter &animation, int objectId )
{

    //FIXME oo doesn't support hide animation object
    if ( effect3 != EF3_NONE || !d_fileName.isEmpty())
    {
        animation.startElement( "presentation:hide-shape" );
        animation.addAttribute( "draw:shape-id", objectId );
        switch( effect3 )
        {
        case EF3_NONE:
            animation.addAttribute( "presentation:effect", "none" );
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

void KPObject::loadOasis(const QDomElement &element, KoOasisContext & context, KPRLoadingInfo *info)
{
    if(element.hasAttribute( "draw:name" ))
       objectName = element.attribute("draw:name");
    orig.setX( KoUnit::parseValue( element.attribute( "svg:x" ) ) );
    orig.setY( KoUnit::parseValue( element.attribute( "svg:y" ) ) );
    ext.setWidth(KoUnit::parseValue( element.attribute( "svg:width" )) );
    ext.setHeight(KoUnit::parseValue( element.attribute( "svg:height" ) ) );
    //kdDebug()<<" orig.x() :"<<orig.x() <<" orig.y() :"<<orig.y() <<"ext.width() :"<<ext.width()<<" ext.height(): "<<ext.height()<<endl;
    KoStyleStack &styleStack = context.styleStack();
    styleStack.setTypeProperties( "" ); //no type default type
    if( element.hasAttribute( "draw:transform" ))
        {
            QString transform = element.attribute( "draw:transform" );
            kdDebug()<<" transform action :"<<transform<<endl;
            if( transform.contains("rotate ("))
                {
                    //kdDebug()<<" rotate object \n";
                    transform = transform.remove("rotate (" );
                    transform = transform.left(transform.find(")"));
                    //kdDebug()<<" transform :"<<transform<<endl;
                    bool ok;
                    double radian = transform.toDouble(&ok);
                    if( ok )
                        {
                            angle = (-1 * ((radian*180)/M_PI));
                        }
                    else
                        angle = 0.0;
                }
        }
    QDomElement *animation = 0L;
    lstAnimation *tmp = 0L;
    if( element.hasAttribute("draw:id"))
    {
        tmp = info->animationShowById(element.attribute("draw:id") );
        if ( tmp )
            animation = tmp->element;
    }

    if( animation)
    {
        QString effectStr = animation->attribute("presentation:effect");
        QString dir = animation->attribute("presentation:direction");
        QString speed = animation->attribute( "presentation:speed" );
        appearStep = tmp->order;
        kdDebug()<<" appear direction : "<<dir<<" effect :"<< effectStr <<" speed :"<<speed<<endl;

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
            kdDebug()<<" speed argument is not defined :"<<speed<<endl;

        if ( animation->hasAttribute("presentation:animation-delay" ) )
        {
            appearTimer = loadOasisTimer(animation->attribute("presentation:animation-delay" ) );
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
                kdDebug()<<" not supported :"<<effectStr<<endl;
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
                kdDebug ()<<" not supported :"<<effectStr<<endl;
        }
        else
            kdDebug()<<" not supported :"<<effectStr<<endl;
        QDomElement sound = animation->namedItem( "presentation:sound" ).toElement();
        if ( !sound.isNull() )
        {
            kdDebug()<<" object has sound effect \n";
            if ( sound.hasAttribute( "xlink:href" ) )
            {
                a_fileName =sound.attribute( "xlink:href" );
                appearSoundEffect = true;
            }
        }
    }

    animation = 0L;
    tmp = 0L;
    if( element.hasAttribute("draw:id"))
    {
        tmp = info->animationHideById(element.attribute("draw:id") );
        if ( tmp )
            animation = tmp->element;
    }

    if( animation)
    {
        QString effectStr = animation->attribute("presentation:effect");
        QString dir = animation->attribute("presentation:direction");
        QString speed = animation->attribute( "presentation:speed" );
        kdDebug()<<" appear direction : "<<dir<<" effect :"<< effectStr <<" speed :"<<speed<<endl;
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
            kdDebug()<<" speed argument is not defined :"<<speed<<endl;

        if ( animation->hasAttribute("presentation:animation-delay" ) )
        {
            disappearTimer = loadOasisTimer(animation->attribute("presentation:animation-delay" ) );
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
                kdDebug()<<" not supported :"<<effectStr<<endl;
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
                kdDebug ()<<" not supported :"<<effectStr<<endl;
        }
        else
            kdDebug()<<" not supported :"<<effectStr<<endl;
        //FIXME allow to save/load this attribute
        if ( effect3 != EF3_NONE )
            disappear = true;
        QDomElement sound = animation->namedItem( "presentation:sound" ).toElement();
        if ( !sound.isNull() )
        {
            kdDebug()<<" object has sound effect \n";
            if ( sound.hasAttribute( "xlink:href" ) )
            {
                d_fileName =sound.attribute( "xlink:href" );
                disappearSoundEffect = true;
            }
        }
    }
    //shadow
#if 0 //move it to kptextobject
    if ( !element.hasAttribute( "type" ) ||
         ( element.hasAttribute( "type" ) && element.attribute( "type" ) == "4" ) )
    {
        kdDebug()<<" text document !!!!!\n";
        if ( styleStack.hasAttribute( "fo:text-shadow" ) &&
             styleStack.attribute( "fo:text-shadow" ) != "none" )
        {
            QString distance = styleStack.attribute( "fo:text-shadow" );
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
    if ( styleStack.hasAttribute("draw:move-protect" ) )
    {
        kdDebug()<<" styleStack.attribute(draw:move-protect ) :"<<styleStack.attribute("draw:move-protect" )<<endl;
        protect = ( styleStack.attribute("draw:move-protect" ) == "true" );
    }
    if ( styleStack.hasAttribute("draw:size-protect" ) )
    {
        kdDebug()<<" styleStack.attribute(draw:size-protect ) :"<<styleStack.attribute("draw:size-protect" )<<endl;
        protect = ( styleStack.attribute("draw:size-protect" ) == "true" );
    }

    //not supported into kpresenter
    if ( styleStack.hasAttribute("draw:textarea-vertical-align" ) )
    {
        kdDebug()<<" styleStack.attribute(draw:textarea-vertical-align ) :"<<styleStack.attribute("draw:textarea-vertical-align" )<<endl;
    }
    if ( styleStack.hasAttribute("draw:textarea-horizontal-align") )
    {
        kdDebug()<<" styleStack.attribute(draw:textarea-horizontal-align ) :"<<styleStack.attribute("draw:textarea-horizontal-align" )<<endl;
    }
    if ( styleStack.hasAttribute( "draw:shadow" ) &&
              styleStack.attribute( "draw:shadow") == "visible" )
    {
        // use the shadow attribute to indicate an object-shadow
        double x = KoUnit::parseValue( styleStack.attribute( "draw:shadow-offset-x" ) );
        double y = KoUnit::parseValue( styleStack.attribute( "draw:shadow-offset-y" ) );
        kdDebug()<<" shadow x : "<<x<<" shadow y :"<<y<<endl;
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
        if ( styleStack.hasAttribute ( "draw:shadow-color" ) )
            shadowColor= QColor(styleStack.attribute( "draw:shadow-color" ) );
        kdDebug()<<" shadow color : "<<shadowColor.name()<<endl;
    }
}

void KPObject::saveOasisShadowElement( KoGenStyle &styleobjectauto ) const
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

double KPObject::load(const QDomElement &element) {

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
            objectName = e.attribute("objectName");
    }
    else {
        objectName = QString::null;
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

KoSize KPObject::getRealSize() const {
    KoSize size = ext;

    if ( angle != 0.0 ) {
      float angInRad = angle * M_PI / 180;
      size.setWidth( ext.width() * fabs( cos( angInRad ) ) + ext.height() * fabs( sin( angInRad ) ) );
      size.setHeight( ext.width() * fabs( sin( angInRad ) ) + ext.height() * fabs( cos( angInRad ) ) );
    }

    return size;
}

KoPoint KPObject::getRealOrig() const {
    KoPoint origin = orig;

    if ( angle != 0.0 ) {
        KoSize dist( ( getRealSize() - ext ) / 2 );
        origin.setX( orig.x() - dist.width() );
        origin.setY( orig.y() - dist.height() );
    }

    return origin;
}

KoRect KPObject::getRealRect() const {
    return KoRect( getRealOrig(), getRealSize() );
}

void KPObject::flip( bool /*horizontal*/ ) {
    // flip the angle
    if ( angle ) {
        angle = 360.0 - angle;
    }
}

KoRect KPObject::getBoundingRect() const
{
    KoRect r( orig, ext );

    if ( shadowDistance > 0 )
    {
        double sx = r.x(), sy = r.y();
        getShadowCoords( sx, sy );
        KoRect r2( sx, sy, r.width(), r.height() );
        r = r.unite( r2 );
    }

    if ( angle == 0.0 )
        return r;
    else
        return rotateRectObject();
}

KoRect KPObject::rotateRectObject() const
{
    KoRect br = KoRect( 0,0, ext.width(), ext.height() );
    double pw = br.width();
    double ph = br.height();
    KoRect rr = br;
    double yPos = -rr.y();
    double xPos = -rr.x();
    rr.moveTopLeft( KoPoint( -rr.width() / 2.0, -rr.height() / 2.0 ) );
    QWMatrix m;
    m.translate( pw / 2.0, ph / 2.0 );
    m.rotate( angle );
    m.translate( rr.left() + xPos, rr.top() + yPos );
    KoRect r = KoRect::fromQRect(m.mapRect( br.toQRect() )); // see above TODO
    r.moveBy( orig.x() , orig.y() );
    return r;
}

void KPObject::rotateObject(QPainter *paint,KoZoomHandler *_zoomHandler)
{
    KoRect rr = KoRect( 0, 0, ext.width(), ext.height() );
    rr.moveTopLeft( KoPoint( -ext.width() / 2.0, -ext.height() / 2.0 ) );
    QWMatrix m;
    m.translate( _zoomHandler->zoomItX(ext.width() / 2.0), _zoomHandler->zoomItY(ext.height() / 2.0 ));
    m.rotate( angle );
    m.translate( _zoomHandler->zoomItX(rr.left()), _zoomHandler->zoomItY(rr.top()) );

    paint->setWorldMatrix( m, true );
}

void KPObject::rotateObjectWithShadow(QPainter *paint,KoZoomHandler *_zoomHandler)
{
    KoRect rr = KoRect( 0, 0, ext.width(), ext.height() );
    rr.moveTopLeft( KoPoint( -ext.width() / 2.0, -ext.height() / 2.0 ) );
    double sx = 0;
    double sy = 0;
    getShadowCoords( sx, sy );

    QWMatrix m;
    m.translate( _zoomHandler->zoomItX(ext.width() / 2.0), _zoomHandler->zoomItY(ext.height() / 2.0) );
    m.rotate( angle );
    m.translate( _zoomHandler->zoomItX(rr.left() + sx), _zoomHandler->zoomItY(rr.top() + sy) );

    paint->setWorldMatrix( m, true );
}

bool KPObject::contains( const KoPoint &_point ) const
{
    if ( angle == 0.0 )
    {
        KoRect r( orig, ext );
        return r.contains( _point );
    }
    else
    {
        KoRect r=rotateRectObject();
        return r.contains( _point );
    }
}

bool KPObject::intersects( const KoRect &_rect ) const
{
    if ( angle == 0.0 )
    {
        KoRect r( orig, ext );
        return r.intersects( _rect );
    }
    else
    {
        KoRect r=rotateRectObject();
        return r.intersects( _rect );
    }
}

QCursor KPObject::getCursor( const KoPoint &_point, ModifyType &_modType,
                             KPresenterDoc *doc ) const
{
    KoZoomHandler * zh = doc->zoomHandler();
    int px = zh->zoomItX(_point.x());
    int py = zh->zoomItY(_point.y());
    int ox = zh->zoomItX(orig.x());
    int oy = zh->zoomItY(orig.y());
    int ow = zh->zoomItX(ext.width());
    int oh = zh->zoomItY(ext.height());

    bool headerFooter=doc->isHeaderFooter(this);
    KoRect r( ox, oy, ow, oh );
    if ( angle != 0.0 )
    {
        QRect rr = zh->zoomRect( rotateRectObject() );
        ox = rr.x();
        oy = rr.y();
        ow = rr.width();
        oh = rr.height();
    }

    int sz = 4;
    if ( px >= ox && py >= oy && px <= ox + QMIN( ow / 3, sz ) && py <= oy + QMIN( oh / 3, sz ) )
    {
        _modType = MT_RESIZE_LU;
        if ( protect)
            return Qt::ForbiddenCursor;
        return Qt::sizeFDiagCursor;
    }

    if ( px >= ox && py >= oy + oh / 2 - QMIN( oh / 6, sz / 2 )
         && px <= ox + QMIN( ow / 3, sz)
         && py <= oy + oh / 2 + QMIN( oh / 6, sz / 2 ) )
    {
        _modType = MT_RESIZE_LF;
        if ( protect)
            return Qt::ForbiddenCursor;
        return Qt::sizeHorCursor;
    }

    if ( px >= ox && py >= oy + oh - QMIN( oh / 3, sz ) && px <= ox + QMIN( ow / 3, sz ) && py <= oy + oh )
    {
        _modType = MT_RESIZE_LD;
        if ( protect)
            return Qt::ForbiddenCursor;
        return Qt::sizeBDiagCursor;
    }

    if ( px >= ox + ow / 2 - QMIN( ow / 6, sz / 2 ) && py >= oy
         && px <= ox + ow / 2 + QMIN( ow / 6, sz / 2 )
         && py <= oy + QMIN( oh / 3, sz ) )
    {
        _modType = MT_RESIZE_UP;
        if ( protect)
            return Qt::ForbiddenCursor;
        return Qt::sizeVerCursor;
    }

    if ( px >= ox + ow / 2 - QMIN( ow / 6, sz / 2 ) && py >= oy + oh - QMIN( oh / 3, sz )
         && px <= ox + ow / 2 + QMIN( ow / 6, sz / 2 ) && py <= oy + oh )
    {
        _modType = MT_RESIZE_DN;
        if ( protect)
            return Qt::ForbiddenCursor;
        return Qt::sizeVerCursor;
    }

    if ( px >= ox + ow - QMIN( ow / 3, sz ) && py >= oy && px <= ox + ow && py <= oy + QMIN( oh / 3, sz) )
    {
        _modType = MT_RESIZE_RU;
        if ( protect)
            return Qt::ForbiddenCursor;
        return Qt::sizeBDiagCursor;
    }

    if ( px >= ox + ow - QMIN( ow / 3, sz ) && py >= oy + oh / 2 - QMIN( oh / 6, sz / 2 )
         && px <= ox + ow && py <= oy + oh / 2 + QMIN( oh / 6, sz / 2) )
    {
        _modType = MT_RESIZE_RT;
        if ( protect)
            return Qt::ForbiddenCursor;
        return Qt::sizeHorCursor;
    }

    if ( px >= ox + ow - QMIN( ow / 3, sz ) && py >= oy + oh - QMIN( oh / 3, sz)
         && px <= ox + ow && py <= oy + oh )
    {
        _modType = MT_RESIZE_RD;
        if ( protect)
            return Qt::ForbiddenCursor;
        return Qt::sizeFDiagCursor;
    }

    //header footer can't move
    if(!headerFooter)
        _modType = MT_MOVE;

    return Qt::sizeAllCursor;
}

void KPObject::getShadowCoords( double& _x, double& _y ) const
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

void KPObject::paintSelection( QPainter *_painter, KoZoomHandler *_zoomHandler, SelectionMode mode )
{
    if ( !selected || mode == SM_NONE )
        return;

    _painter->save();
    _painter->translate( _zoomHandler->zoomItX(orig.x()), _zoomHandler->zoomItY(orig.y()) );
    _painter->setPen( QPen( Qt::black, 1, QPen::SolidLine ) );
    _painter->setBrush( kapp->palette().color( QPalette::Active, QColorGroup::Highlight ) );

    KoRect r = rotateRectObject();
    int x = _zoomHandler->zoomItX( r.left() - orig.x());
    int y = _zoomHandler->zoomItY( r.top() - orig.y());
    int zX6 = /*_zoomHandler->zoomItX(*/ 6 ;
    int zY6 = /*_zoomHandler->zoomItY(*/ 6 ;
    int w = _zoomHandler->zoomItX(r.width()) - 6;
    int h = _zoomHandler->zoomItY(r.height()) - 6;

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

void KPObject::doDelete()
{
    if ( cmds == 0 && !inObjList )
        delete this;
}

DCOPObject* KPObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPresenterObjectIface( this );

    return dcop;
}

void KPObject::setupClipRegion( QPainter *painter, const QRegion &clipRegion )
{
    QRegion region = painter->clipRegion( QPainter::CoordPainter );
    if ( region.isEmpty() )
        region = clipRegion;
    else
        region.unite( clipRegion );

    painter->setClipRegion( region, QPainter::CoordPainter );
}

QDomElement KPObject::createValueElement(const QString &tag, int value, QDomDocument &doc) {
    QDomElement elem=doc.createElement(tag);
    elem.setAttribute(attrValue, value);
    return elem;
}

QDomElement KPObject::createGradientElement(const QString &tag, const QColor &c1, const QColor &c2,
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

void KPObject::toGradient(const QDomElement &element, QColor &c1, QColor &c2, BCType &type,
                          bool &unbalanced, int &xfactor, int &yfactor) const {
    c1=retrieveColor(element, attrC1, "red1", "green1", "blue1");
    c2=retrieveColor(element, attrC2, "red2", "green2", "blue2");
    if(element.hasAttribute(attrType))
        type=static_cast<BCType>(element.attribute(attrType).toInt());
    if(element.hasAttribute(attrUnbalanced))
        unbalanced=static_cast<bool>(element.attribute(attrUnbalanced).toInt());
    if(element.hasAttribute(attrXFactor))
        xfactor=element.attribute(attrXFactor).toInt();
    if(element.hasAttribute(attrYFactor))
        yfactor=element.attribute(attrYFactor).toInt();
}

QDomElement KPObject::createPenElement(const QString &tag, const QPen &pen, QDomDocument &doc) {

    QDomElement elem=doc.createElement(tag);
    elem.setAttribute(attrColor, pen.color().name());
    elem.setAttribute(attrWidth, pen.width());
    elem.setAttribute(attrStyle, static_cast<int>(pen.style()));
    return elem;
}

QPen KPObject::toPen(const QDomElement &element) const {

    QPen pen;
    pen.setColor(retrieveColor(element));
    if(element.hasAttribute(attrStyle))
        pen.setStyle(static_cast<Qt::PenStyle>(element.attribute(attrStyle).toInt()));
    if(element.hasAttribute(attrWidth))
        pen.setWidth(element.attribute(attrWidth).toInt());
    return pen;
}

QDomElement KPObject::createBrushElement(const QString &tag, const QBrush &brush, QDomDocument &doc) {

    QDomElement elem=doc.createElement(tag);
    elem.setAttribute(attrColor, brush.color().name());
    elem.setAttribute(attrStyle, static_cast<int>(brush.style()));
    return elem;
}

QBrush KPObject::toBrush(const QDomElement &element) const {

    QBrush brush;
    brush.setColor(retrieveColor(element));
    if(element.hasAttribute(attrStyle))
        brush.setStyle(static_cast<Qt::BrushStyle>(element.attribute(attrStyle).toInt()));
    return brush;
}

QColor KPObject::retrieveColor(const QDomElement &element, const QString &cattr,
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

void KPObject::draw( QPainter *_painter, KoZoomHandler*_zoomHandler,
                     int /*pageNum*/, SelectionMode selectionMode, bool drawContour )
{
    if ( selectionMode != SM_NONE && !drawContour )
        paintSelection( _painter, _zoomHandler, selectionMode );
}

QPen KPObject::getPen() const
{
    // Return the default pen
    return QPen();
}

void KPObject::getRealSizeAndOrigFromPoints( KoPointArray &points, float angle,
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

KPShadowObject::KPShadowObject()
    : KPObject()
{
}

KPShadowObject::KPShadowObject( const QPen &_pen )
    : KPObject(), pen( _pen )
{
}

KPShadowObject::KPShadowObject( const QPen &_pen, const QBrush &_brush )
    : KPObject(), pen( _pen ), brush( _brush )
{
}

KPShadowObject &KPShadowObject::operator=( const KPShadowObject & )
{
    return *this;
}

QDomDocumentFragment KPShadowObject::save( QDomDocument& doc,double offset )
{
    QDomDocumentFragment fragment=KPObject::save(doc, offset);

    if(pen!=defaultPen())
        fragment.appendChild(KPObject::createPenElement(tagPEN, pen, doc));
    if(brush.color()!=Qt::black || brush.style()!=Qt::NoBrush)
        fragment.appendChild(KPObject::createBrushElement(tagBRUSH, brush, doc));
    return fragment;
}

void KPShadowObject::saveOasisStrokeElement( KoGenStyles& mainStyles, KoGenStyle &styleobjectauto ) const
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
        }
        styleobjectauto.addProperty( "svg:stroke-color", pen.color().name() );
        styleobjectauto.addPropertyPt( "svg:stroke-width", ( int )pen.width() );
    }
}

QString KPShadowObject::saveOasisStrokeStyle( KoGenStyles& mainStyles ) const
{
    KoGenStyle stroke( KPresenterDoc::STYLE_STROKE /*, "graphic"*/ /*no name*/ );
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
    }
    return mainStyles.lookup( stroke, "stroke" );
    //    <draw:stroke-dash draw:name="Fine Dotted" draw:style="rect" draw:dots1="1" draw:distance="0.457cm"/>
}

void KPShadowObject::loadOasis(const QDomElement &element, KoOasisContext & context, KPRLoadingInfo *info)
{
    kdDebug()<<"void KPShadowObject::loadOasis(const QDomElement &element)**********************\n";
    KPObject::loadOasis(element, context, info);
    KoStyleStack &styleStack = context.styleStack();
    styleStack.setTypeProperties( "graphic" );
    if ( styleStack.hasAttribute( "draw:stroke" ))
    {
        if ( styleStack.attribute( "draw:stroke" ) == "none" )
            pen.setStyle(Qt::NoPen );
        else if ( styleStack.attribute( "draw:stroke" ) == "solid" )
            pen.setStyle(Qt::SolidLine );
        else if ( styleStack.attribute( "draw:stroke" ) == "dash" )
        {
            QString style = styleStack.attribute( "draw:stroke-dash" );

            kdDebug()<<" stroke style is  : "<<style<<endl;
            //type not defined by default
            //try to use style.
            QDomElement* draw = context.oasisStyles().drawStyles()[style];
            kdDebug()<<" stroke have oasis style defined :"<<draw<<endl;
            if ( draw )
            {
                //FIXME
                if ( draw->attribute( "draw:style" )=="rect" )
                {
                    if ( draw->attribute( "draw:dots1" )=="1" &&
                         draw->attribute( "draw:dots2" )=="1" &&
                         draw->attribute( "draw:dots1-length" )=="0.508cm" &&
                         draw->attribute( "draw:dots2-length" )=="0.508cm" &&
                         draw->attribute( "draw:distance" )=="0.508cm" )
                        pen.setStyle( Qt::DashLine );
                    else if ( draw->attribute( "draw:dots1" )=="1" &&
                              draw->attribute( "draw:distance" )=="0.257cm" )
                        pen.setStyle(Qt::DotLine );
                    else if ( draw->attribute( "draw:dots1" )=="1" &&
                         draw->attribute( "draw:dots2" )=="1" &&
                         draw->attribute( "draw:dots1-length" )=="0.051cm" &&
                         draw->attribute( "draw:dots2-length" )=="0.254cm" &&
                         draw->attribute( "draw:distance" )=="0.127cm" )
                        pen.setStyle(Qt::DashDotLine );
                    else if ( draw->attribute( "draw:dots1" )=="1" &&
                         draw->attribute( "draw:dots2" )=="2" &&
                         draw->attribute( "draw:dots1-length" )=="0.203cm" &&
                         draw->attribute( "draw:distance" )=="0.203cm" )
                        pen.setStyle(Qt::DashDotDotLine );
                    else
                    {
                        kdDebug()<<" stroke style undefined \n";
                        pen.setStyle(Qt::SolidLine );
                    }

                }
            }
        }
        //FIXME witdh pen style is not good :(
        if ( styleStack.hasAttribute( "svg:stroke-width" ) )
            pen.setWidth( (int) KoUnit::parseValue( styleStack.attribute( "svg:stroke-width" ) ) );
        if ( styleStack.hasAttribute( "svg:stroke-color" ) )
            pen.setColor( styleStack.attribute( "svg:stroke-color" ) );
    }
    else
        pen = defaultPen();
    kdDebug()<<"pen style :"<<pen<<endl;
}

double KPShadowObject::load(const QDomElement &element)
{
    double offset=KPObject::load(element);
    QDomElement e=element.namedItem(tagPEN).toElement();
    if(!e.isNull())
        setPen(KPObject::toPen(e));
    else
        pen = defaultPen();
    e=element.namedItem(tagBRUSH).toElement();
    if(!e.isNull())
        setBrush(KPObject::toBrush(e));
    else
        brush=QBrush();
    e=element.namedItem(tagFILLTYPE).toElement();
    return offset;
}

void KPShadowObject::draw( QPainter *_painter, KoZoomHandler*_zoomHandler,
                           int pageNum, SelectionMode selectionMode, bool drawContour )
{
    double ox = orig.x();
    double oy = orig.y();
    _painter->save();

    // Draw the shadow if any
    if ( shadowDistance > 0 && !drawContour )
    {
        _painter->save();
        QPen tmpPen( pen );
        pen.setColor( shadowColor );
        QBrush tmpBrush( brush );
        brush.setColor( shadowColor );

        if ( angle == 0 )
        {
            double sx = ox;
            double sy = oy;
            getShadowCoords( sx, sy );

            _painter->translate( _zoomHandler->zoomItX( sx ), _zoomHandler->zoomItY( sy ) );
            paint( _painter, _zoomHandler, pageNum, true, drawContour );
        }
        else
        {
            _painter->translate( _zoomHandler->zoomItX(ox), _zoomHandler->zoomItY(oy) );
            rotateObjectWithShadow(_painter, _zoomHandler);
            paint( _painter, _zoomHandler, pageNum, true, drawContour );
        }

        pen = tmpPen;
        brush = tmpBrush;
        _painter->restore();
    }

    _painter->translate( _zoomHandler->zoomItX(ox), _zoomHandler->zoomItY(oy) );

    if ( angle != 0 )
        rotateObject(_painter,_zoomHandler);
    paint( _painter, _zoomHandler, pageNum, false, drawContour );

    _painter->restore();

    KPObject::draw( _painter, _zoomHandler, pageNum, selectionMode, drawContour );
}

QPen KPShadowObject::defaultPen() const
{
    return QPen();
}

KP2DObject::KP2DObject()
    : KPShadowObject(), gColor1( Qt::red ), gColor2( Qt::green )
{
    gradient = 0;
    fillType = FT_BRUSH;
    gType = BCT_GHORZ;
    unbalanced = false;
    xfactor = 100;
    yfactor = 100;
}

KP2DObject::KP2DObject( const QPen &_pen, const QBrush &_brush, FillType _fillType,
                        const QColor &_gColor1, const QColor &_gColor2, BCType _gType,
                        bool _unbalanced, int _xfactor, int _yfactor )
    : KPShadowObject( _pen, _brush ), gColor1( _gColor1 ), gColor2( _gColor2 )
{
    gType = _gType;
    fillType = _fillType;
    unbalanced = _unbalanced;
    xfactor = _xfactor;
    yfactor = _yfactor;

    if ( fillType == FT_GRADIENT )
        gradient = new KPGradient( gColor1, gColor2, gType, unbalanced, xfactor, yfactor );
    else
        gradient = 0;
}

void KP2DObject::setFillType( FillType _fillType )
{
    fillType = _fillType;

    if ( fillType == FT_BRUSH && gradient )
    {
        delete gradient;
        gradient = 0;
    }
    if ( fillType == FT_GRADIENT && !gradient )
        gradient = new KPGradient( gColor1, gColor2, gType, unbalanced, xfactor, yfactor );
}

QDomDocumentFragment KP2DObject::save( QDomDocument& doc,double offset )
{
    QDomDocumentFragment fragment=KPShadowObject::save(doc, offset);
    if(fillType!=FT_BRUSH)
        fragment.appendChild(KPObject::createValueElement(tagFILLTYPE, static_cast<int>(fillType), doc));
    if(gColor1!=Qt::red || gColor2!=Qt::green || gType!=BCT_GHORZ || unbalanced || xfactor!=100 || yfactor!=100)
        fragment.appendChild(KPObject::createGradientElement(tagGRADIENT, gColor1, gColor2, static_cast<int>(gType),
                                                             unbalanced, xfactor, yfactor, doc));
    return fragment;
}

QString KP2DObject::saveOasisBackgroundStyle( KoXmlWriter &xmlWriter, KoGenStyles& mainStyles, int indexObj ) const
{
    saveOasisPosObject( xmlWriter,indexObj );
    KoGenStyle styleobjectauto;
    if ( sticky )
        styleobjectauto = KoGenStyle( KPresenterDoc::STYLE_PRESENTATIONSTICKYOBJECT, "presentation" );
    else
        styleobjectauto = KoGenStyle( KPresenterDoc::STYLE_GRAPHICAUTO, "graphic" );
    switch ( fillType )
    {
    case FT_BRUSH:
        //todo FIXME when text object doesn't have a background
        if(brush.color()!=Qt::black || brush.style()!=Qt::NoBrush)
        {
            if ( brush.style() == Qt::SolidPattern )
            {
                styleobjectauto.addProperty( "draw:fill","solid" );
                styleobjectauto.addProperty( "draw:fill-color", brush.color().name() );
            }
            else if ( brush.style() == Qt::Dense1Pattern )
            {
                styleobjectauto.addProperty( "draw:transparency", "94%" );
                styleobjectauto.addProperty( "draw:fill","solid" );
                styleobjectauto.addProperty( "draw:fill-color", brush.color().name() );
            }
            else if ( brush.style() == Qt::Dense2Pattern )
            {
                styleobjectauto.addProperty( "draw:transparency", "88%" );
                styleobjectauto.addProperty( "draw:fill","solid" );
                styleobjectauto.addProperty( "draw:fill-color", brush.color().name() );
            }
            else if ( brush.style() == Qt::Dense3Pattern )
            {
                styleobjectauto.addProperty( "draw:transparency", "63%" );
                styleobjectauto.addProperty( "draw:fill","solid" );
                styleobjectauto.addProperty( "draw:fill-color", brush.color().name() );
            }
            else if ( brush.style() == Qt::Dense4Pattern )
            {
                styleobjectauto.addProperty( "draw:transparency", "50%" );
                styleobjectauto.addProperty( "draw:fill","solid" );
                styleobjectauto.addProperty( "draw:fill-color", brush.color().name() );
            }
            else if ( brush.style() == Qt::Dense5Pattern )
            {
                styleobjectauto.addProperty( "draw:transparency", "37%" );
                styleobjectauto.addProperty( "draw:fill","solid" );
                styleobjectauto.addProperty( "draw:fill-color", brush.color().name() );
            }
            else if ( brush.style() == Qt::Dense6Pattern )
            {
                styleobjectauto.addProperty( "draw:transparency", "12%" );
                styleobjectauto.addProperty( "draw:fill","solid" );
                styleobjectauto.addProperty( "draw:fill-color", brush.color().name() );
            }
            else if ( brush.style() == Qt::Dense7Pattern )
            {
                styleobjectauto.addProperty( "draw:transparency", "6%" );
                styleobjectauto.addProperty( "draw:fill","solid" );
                styleobjectauto.addProperty( "draw:fill-color", brush.color().name() );
            }
            else //otherstyle
            {
                styleobjectauto.addProperty( "draw:fill","hatch" );
                styleobjectauto.addProperty( "draw:fill-hatch-name", saveOasisHatchStyle( mainStyles ) );
            }
        }
        else
        {
            styleobjectauto.addProperty( "draw:fill","none" );
        }
        break;
    case FT_GRADIENT:
        styleobjectauto.addProperty( "draw:fill","gradient" );
        styleobjectauto.addProperty( "draw:fill-gradient-name", saveOasisGradientStyle( mainStyles ) );
        break;
    }
    saveOasisObjectProtectStyle( styleobjectauto );

    saveOasisStrokeElement( mainStyles, styleobjectauto );
    saveOasisMarginElement( styleobjectauto );
    saveOasisShadowElement( styleobjectauto );
    saveOasisPictureElement( styleobjectauto );

    if ( sticky )
        return mainStyles.lookup( styleobjectauto, "pr" );
    else
        return mainStyles.lookup( styleobjectauto, "gr" );
}

QString KP2DObject::saveOasisHatchStyle( KoGenStyles& mainStyles ) const
{
    KoGenStyle hatchStyle( KPresenterDoc::STYLE_HATCH /*no family name*/);
    hatchStyle.addAttribute( "draw:color", brush.color().name() );
    //hatchStyle.addAttribute( "draw:distance", m_distance ); not implemented into kpresenter
    switch( brush.style() )
    {
    case Qt::HorPattern:
        hatchStyle.addAttribute( "draw:style", "single" );
        hatchStyle.addAttribute( "draw:rotation", 0);
        break;
    case Qt::BDiagPattern:
        hatchStyle.addAttribute( "draw:style", "single" );
        hatchStyle.addAttribute( "draw:rotation", 450);
        break;
    case Qt::VerPattern:
        hatchStyle.addAttribute( "draw:style", "single" );
        hatchStyle.addAttribute( "draw:rotation", 900);
        break;
    case Qt::FDiagPattern:
        hatchStyle.addAttribute( "draw:style", "single" );
        hatchStyle.addAttribute( "draw:rotation", 1350);
        break;
    case Qt::CrossPattern:
        hatchStyle.addAttribute( "draw:style", "double" );
        hatchStyle.addAttribute( "draw:rotation", 0);
        break;
    case Qt::DiagCrossPattern:
        hatchStyle.addAttribute( "draw:style", "double" );
        hatchStyle.addAttribute( "draw:rotation", 450);
        break;
    default:
        break;
    }

    return mainStyles.lookup( hatchStyle, "hatch" );
}


QString KP2DObject::saveOasisGradientStyle( KoGenStyles& mainStyles ) const
{
    KoGenStyle gradientStyle( KPresenterDoc::STYLE_GRADIENT /*no family name*/);
    gradientStyle.addAttribute( "draw:start-color", gColor1.name() );
    gradientStyle.addAttribute( "draw:end-color", gColor2.name() );

    QString unbalancedx( "50%" );
    QString unbalancedy( "50%" );

    if ( unbalanced )
    {
        unbalancedx = QString( "%1%" ).arg( xfactor / 4 + 50 );
        unbalancedy = QString( "%1%" ).arg( yfactor / 4 + 50 );
    }
    gradientStyle.addAttribute( "draw:cx", unbalancedx );
    gradientStyle.addAttribute( "draw:cy", unbalancedy );

    switch( gType )
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
#if 0
    if ( draw )
    {
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
                gType = BCT_GHORZ; // horizontal
            else if ( nearAngle == 90 || nearAngle == 270 )
                gType = BCT_GVERT; // vertical
            else if ( nearAngle == 45 || nearAngle == 225 )
                gType = BCT_GDIAGONAL1; // diagonal 1
            else if ( nearAngle == 135 || nearAngle == 315 )
                gType = BCT_GDIAGONAL2; // diagonal 2
        }
        else if ( type == "radial" || type == "ellipsoid" )
            gType = BCT_GCIRCLE; // circle
        else if ( type == "square" || type == "rectangular" )
            gType = BCT_GRECT; // rectangle
        else if ( type == "axial" )
            gType = BCT_GPIPECROSS; // pipecross
        else //safe
            gType = BCT_PLAIN; // plain

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
            unbalanced =  0;
            xfactor =  100;
            yfactor= 100;
        }
        else
        {
            unbalanced =  1;
            // map 0 - 100% to -200 - 200
            xfactor = ( 4 * x - 200 );
            yfactor = ( 4 * y - 200 );
        }
    }
    tmpBrush.setStyle(static_cast<Qt::BrushStyle>( 1 ) );
    setBrush( tmpBrush );
    setFillType(FT_GRADIENT );

#endif
    return mainStyles.lookup( gradientStyle, "gradient" );
}


void KP2DObject::loadOasis(const QDomElement &element, KoOasisContext & context, KPRLoadingInfo *info)
{
    kdDebug()<<"void KP2DObject::loadOasis(const QDomElement &element)\n";
    QBrush tmpBrush;

    KPShadowObject::loadOasis(element, context, info);
    KoStyleStack &styleStack = context.styleStack();
    styleStack.setTypeProperties( "graphic" );


    if ( styleStack.hasAttribute( "draw:fill" ) )
    {
        const QString fill = styleStack.attribute( "draw:fill" );
        kdDebug()<<" load object gradient fill type :"<<fill<<endl;

        if ( fill == "solid" )
        {
            tmpBrush.setStyle(static_cast<Qt::BrushStyle>( 1 ) );
            if ( styleStack.hasAttribute( "draw:fill-color" ) )
                tmpBrush.setColor(styleStack.attribute( "draw:fill-color" ) );
            if ( styleStack.hasAttribute( "draw:transparency" ) )
            {
                QString transparency = styleStack.attribute( "draw:transparency" );
                if ( transparency == "94%" )
                {
                    tmpBrush.setStyle(Qt::Dense1Pattern);
                }
                else if ( transparency == "88%" )
                {
                    tmpBrush.setStyle(Qt::Dense2Pattern);
                }
                else if ( transparency == "63%" )
                {
                    tmpBrush.setStyle(Qt::Dense3Pattern);

                }
                else if ( transparency == "50%" )
                {
                    tmpBrush.setStyle(Qt::Dense4Pattern);

                }
                else if ( transparency == "37%" )
                {
                    tmpBrush.setStyle(Qt::Dense5Pattern);

                }
                else if ( transparency == "12%" )
                {
                    tmpBrush.setStyle(Qt::Dense6Pattern);

                }
                else if ( transparency == "6%" )
                {
                    tmpBrush.setStyle(Qt::Dense7Pattern);

                }
                else
                    kdDebug()<<" transparency is not defined into kpresenter :"<<transparency<<endl;
            }
            setBrush(tmpBrush );
        }
        else if ( fill == "hatch" )
        {
            QString style = styleStack.attribute( "draw:fill-hatch-name" );
            kdDebug()<<" hatch style is  : "<<style<<endl;

            //type not defined by default
            //try to use style.
            QDomElement* draw = context.oasisStyles().drawStyles()[style];
            if ( draw)
            {
                kdDebug()<<"We have a style";
                int angle = 0;
                if( draw->hasAttribute( "draw:rotation" ))
                {
                    angle = (draw->attribute( "draw:rotation" ).toInt())/10;
                    kdDebug()<<"angle :"<<angle<<endl;
                }
                if(draw->hasAttribute( "draw:color" ) )
                {
                    //kdDebug()<<" draw:color :"<<draw->attribute( "draw:color" )<<endl;
                    tmpBrush.setColor(draw->attribute( "draw:color" ) );
                }
                if( draw->hasAttribute( "draw:distance" ))
                {
                    //todo implemente it into kpresenter
                }
                if( draw->hasAttribute("draw:display-name"))
                {
                    //todo implement it into kpresenter
                }
                if( draw->hasAttribute( "draw:style" ))
                {
                    //todo implemente it into kpresenter
                    QString styleHash = draw->attribute( "draw:style" );
                    if( styleHash == "single")
                    {
                        switch( angle )
                        {
                        case 0:
                        case 180:
                            tmpBrush.setStyle(Qt::HorPattern );
                            break;
                        case 45:
                        case 225:
                            tmpBrush.setStyle(Qt::BDiagPattern );
                            break;
                        case 90:
                        case 270:
                            tmpBrush.setStyle(Qt::VerPattern );
                            break;
                        case 135:
                        case 315:
                            tmpBrush.setStyle(Qt::FDiagPattern );
                            break;
                        default:
                            //todo fixme when we will have a kopaint
                            kdDebug()<<" draw:rotation 'angle' : "<<angle<<endl;
                            break;
                        }
                    }
                    else if( styleHash == "double")
                    {
                        switch( angle )
                        {
                        case 0:
                        case 180:
                        case 90:
                        case 270:
                            tmpBrush.setStyle(Qt::CrossPattern );
                            break;
                        case 45:
                        case 135:
                        case 225:
                        case 315:
                            tmpBrush.setStyle(Qt::DiagCrossPattern );
                            break;
                        default:
                            //todo fixme when we will have a kopaint
                            kdDebug()<<" draw:rotation 'angle' : "<<angle<<endl;
                            break;
                        }

                    }
                    else if( styleHash == "triple")
                    {
                        kdDebug()<<" it is not implemented :( \n";
                    }
                }
            }
            setBrush( tmpBrush );
        }
        else if ( fill == "gradient" )
        {
            // We have to set a brush with brushstyle != no background fill
            // otherwise the properties dialog for the object won't
            // display the preview for the gradient.

            QString style = styleStack.attribute( "draw:fill-gradient-name" );
            kdDebug()<<" style gradient name :"<<style<<endl;
            QDomElement* draw = context.oasisStyles().drawStyles()[style];
            kdDebug()<<" draw : "<<draw<<endl;

            if ( draw )
            {
                gColor1 =  draw->attribute( "draw:start-color" );
                gColor2 = draw->attribute( "draw:end-color" );

                QString type = draw->attribute( "draw:style" );
                kdDebug()<<" type :"<<type<<endl;
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
                        gType = BCT_GHORZ; // horizontal
                    else if ( nearAngle == 90 || nearAngle == 270 )
                        gType = BCT_GVERT; // vertical
                    else if ( nearAngle == 45 || nearAngle == 225 )
                        gType = BCT_GDIAGONAL1; // diagonal 1
                    else if ( nearAngle == 135 || nearAngle == 315 )
                        gType = BCT_GDIAGONAL2; // diagonal 2
                }
                else if ( type == "radial" || type == "ellipsoid" )
                    gType = BCT_GCIRCLE; // circle
                else if ( type == "square" || type == "rectangular" )
                    gType = BCT_GRECT; // rectangle
                else if ( type == "axial" )
                    gType = BCT_GPIPECROSS; // pipecross
                else //safe
                    gType = BCT_PLAIN; // plain

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
                    unbalanced =  0;
                    xfactor =  100;
                    yfactor= 100;
                }
                else
                {
                    unbalanced =  1;
                    // map 0 - 100% to -200 - 200
                    xfactor = ( 4 * x - 200 );
                    yfactor = ( 4 * y - 200 );
                }
            }
            tmpBrush.setStyle(static_cast<Qt::BrushStyle>( 1 ) );
            setBrush( tmpBrush );
            setFillType(FT_GRADIENT );
        }
        else if ( fill == "none" )
        {
            //nothing
        }
        else if ( fill == "bitmap" )
        {
            //todo
            //not implementer into kpresenter...
            //the drawing object is filled with the bitmap specified by the draw:fill-image-name attribute.
            //QBrush implement setPixmap
            //easy just add pixmap and store it.
        }

    }

}

double KP2DObject::load(const QDomElement &element)
{
    double offset=KPShadowObject::load(element);

    QDomElement e=element.namedItem(tagFILLTYPE).toElement();
    if(!e.isNull()) {
        if(e.hasAttribute(attrValue))
            setFillType(static_cast<FillType>(e.attribute(attrValue).toInt()));
    }
    else
        setFillType(FT_BRUSH);

    e=element.namedItem(tagGRADIENT).toElement();
    if(!e.isNull()) {
        KPObject::toGradient(e, gColor1, gColor2, gType, unbalanced, xfactor, yfactor);
        if(gradient)
            gradient->setParameters(gColor1, gColor2, gType, unbalanced, xfactor, yfactor);
    }
    else {
        gColor1=Qt::red;
        gColor2=Qt::green;
        gType=BCT_GHORZ;
        unbalanced=false;
        xfactor=100;
        yfactor=100;
    }
    return offset;
}

void KP2DObject::flip( bool horizontal ) {
    KPObject::flip( horizontal );

    // flip the gradient
    if ( fillType == FT_GRADIENT ) {
        if ( gType == BCT_GDIAGONAL1 ) {
            gType = BCT_GDIAGONAL2;
        }
        else if ( gType == BCT_GDIAGONAL2 ) {
            gType = BCT_GDIAGONAL1;
        }
        if ( ( horizontal && gType == BCT_GDIAGONAL1 ) ||
             ( horizontal && gType == BCT_GDIAGONAL2 ) ||
             ( horizontal && gType == BCT_GHORZ ) ||
             ( ! horizontal && gType == BCT_GVERT ) ) {
            QColor gColorTemp;
            gColorTemp = gColor1;
            gColor1 = gColor2;
            gColor2 = gColorTemp;
        }
        delete gradient;
        gradient = new KPGradient( gColor1, gColor2, gType, unbalanced, xfactor, yfactor );
    }
}
