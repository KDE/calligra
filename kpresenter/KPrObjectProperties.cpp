// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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

#include "KPrObjectProperties.h"

#include "KPrObject.h"
#include "KPrCubicBezierCurveObject.h"
#include "KPrGroupObject.h"
#include "KPrLineObject.h"
#include "KPrRectObject.h"
#include "KPrPolygonObject.h"
#include "KPrPieObject.h"
#include "KPrPixmapObject.h"
#include "KPrPointObject.h"
#include "KPrQuadricBezierCurveObject.h"
#include "KPrTextObject.h"

KPObjectProperties::KPObjectProperties( const QPtrList<KPObject> &objects )
: m_objects( objects )
, m_flags( 0 )
, m_pen( KPPen( Qt::black, 1.0, Qt::SolidLine ), L_NORMAL, L_NORMAL )
, m_protectContent( STATE_UNDEF )
{
    getProperties( m_objects );
}


KPObjectProperties::~KPObjectProperties()
{
}


void KPObjectProperties::getProperties( const QPtrList<KPObject> &objects )
{
    QPtrListIterator<KPObject> it( objects );
    for ( ; it.current() ; ++it )
    {
        switch ( it.current()->getType() )
        {
            case OT_LINE:
                getPenProperties( it.current() );
                getLineEndsProperties( it.current() );
                m_flags |= PtOther;
                break;
            case OT_FREEHAND:
            case OT_POLYLINE:
                getPenProperties( it.current() );
                if( !static_cast<KPPointObject*>( it.current() )->isClosed() )
                    getLineEndsProperties( it.current() );
                m_flags |= PtOther;
                break;
            case OT_QUADRICBEZIERCURVE:
                getPenProperties( it.current() );
                if ( !static_cast<KPQuadricBezierCurveObject*>( it.current() )->isClosed() )
                    getLineEndsProperties( it.current() );
                m_flags |= PtOther;
                break;
            case OT_CUBICBEZIERCURVE:
                getPenProperties( it.current() );
                if ( !static_cast<KPCubicBezierCurveObject*>( it.current() )->isClosed() )
                    getLineEndsProperties( it.current() );
                m_flags |= PtOther;
                break;
            case OT_PIE:
                getPieProperties( it.current() );
                break;
            case OT_RECT:
                getRectProperties( it.current() );
                break;
            case OT_POLYGON:
                getPolygonSettings( it.current() );
                break;
            case OT_TEXT:
                getTextProperties( it.current() );
                break;
            case OT_PART:
            case OT_ELLIPSE:
            case OT_CLOSED_LINE:
            case OT_AUTOFORM:
                getPenProperties( it.current() );
                getBrushProperties( it.current() );
                m_flags |= PtOther;
                break;
            case OT_CLIPART:
            case OT_PICTURE:
                getPictureProperties( it.current() );
                break;
            case OT_GROUP:
                {
                    KPGroupObject *obj = dynamic_cast<KPGroupObject*>( it.current() );
                    if ( obj )
                    {
                        getProperties( obj->objectList() );
                    }
                }
                break;
            default:
                break;
        }
    }
}


void KPObjectProperties::getPenProperties( KPObject *object )
{
    if ( !( m_flags & PtPen ) )
    {
        KPShadowObject *obj = dynamic_cast<KPShadowObject*>( object );
        if ( obj )
        {
            m_pen.pen = obj->getPen();

            m_flags |= PtPen;
        }
    }
}


void KPObjectProperties::getLineEndsProperties( KPObject *object )
{
    if ( !( m_flags & PtLineEnds ) )
    {
        switch ( object->getType() )
        {
            case OT_LINE:
                {
                    KPLineObject *obj = dynamic_cast<KPLineObject*>( object );
                    if ( obj )
                    {
                        m_pen.lineBegin = obj->getLineBegin();
                        m_pen.lineEnd = obj->getLineEnd();

                        m_flags |= PtLineEnds;
                    }
                    break;
                }
            case OT_FREEHAND:
            case OT_POLYLINE:
            case OT_QUADRICBEZIERCURVE:
            case OT_CUBICBEZIERCURVE:
                {
                    KPPointObject *obj = dynamic_cast<KPPointObject*>( object );
                    if ( obj )
                    {
                        m_pen.lineBegin = obj->getLineBegin();
                        m_pen.lineEnd = obj->getLineEnd();

                        m_flags |= PtLineEnds;
                    }
                    break;
                }
            case OT_PIE:
                {
                    KPPieObject *obj = dynamic_cast<KPPieObject*>( object );
                    if ( obj )
                    {
                        m_pen.lineBegin = obj->getLineBegin();
                        m_pen.lineEnd = obj->getLineEnd();

                        m_flags |= PtLineEnds;
                    }
                    break;
                }
            default:
                break;
        }
    }
}


void KPObjectProperties::getBrushProperties( KPObject *object )
{
    if ( !( m_flags & PtBrush ) )
    {
        KP2DObject * obj = dynamic_cast<KP2DObject*>( object );
        if ( obj )
        {
            m_brush.brush = obj->getBrush();
            m_brush.fillType = obj->getFillType();
            m_brush.gColor1 = obj->getGColor1();
            m_brush.gColor2 = obj->getGColor2();
            m_brush.gType = obj->getGType();
            m_brush.unbalanced = obj->getGUnbalanced();
            m_brush.xfactor = obj->getGXFactor();
            m_brush.yfactor = obj->getGYFactor();

            m_flags |= PtBrush;
        }
    }
}


void KPObjectProperties::getRectProperties( KPObject *object )
{
    if ( !( m_flags & PtRectangle ) )
    {
        KPRectObject *obj = dynamic_cast<KPRectObject*>( object );
        if ( obj )
        {
            obj->getRnds( m_rectValues.xRnd, m_rectValues.yRnd );

            getPenProperties( object );
            getBrushProperties( object );
            m_flags |= PtRectangle;
        }
    }
}


void KPObjectProperties::getPolygonSettings( KPObject *object )
{
    if ( !( m_flags & PtPolygon ) )
    {
        KPPolygonObject *obj = dynamic_cast<KPPolygonObject*>( object );
        if ( obj )
        {
            m_polygonSettings.checkConcavePolygon = obj->getCheckConcavePolygon();
            m_polygonSettings.cornersValue = obj->getCornersValue();
            m_polygonSettings.sharpnessValue = obj->getSharpnessValue();

            getPenProperties( object );
            getBrushProperties( object );
            m_flags |= PtPolygon;
        }
    }
}


void KPObjectProperties::getPieProperties( KPObject *object )
{
    if ( !( m_flags & PtPie ) )
    {
        KPPieObject *obj = dynamic_cast<KPPieObject*>( object );
        if ( obj )
        {
            m_pieValues.pieType = obj->getPieType();
            m_pieValues.pieAngle = obj->getPieAngle();
            m_pieValues.pieLength = obj->getPieLength();

            getPenProperties( object );
            if ( obj->getPieType() != PT_ARC )
            {
                getBrushProperties( object );
            }
            else
            {
                getLineEndsProperties( object );
            }

            m_flags |= PtPie;
        }
    }
}


void KPObjectProperties::getPictureProperties( KPObject *object )
{
    if ( !( m_flags & PtPicture ) )
    {
        KPPixmapObject *obj = dynamic_cast<KPPixmapObject*>( object );
        if ( obj )
        {
            m_pictureSettings.mirrorType = obj->getPictureMirrorType();
            m_pictureSettings.depth = obj->getPictureDepth();
            m_pictureSettings.swapRGB = obj->getPictureSwapRGB();
            m_pictureSettings.grayscal = obj->getPictureGrayscal();
            m_pictureSettings.bright = obj->getPictureBright();
            m_pixmap = obj->getOriginalPixmap();

            getPenProperties( object );
            getBrushProperties( object );
            m_flags |= PtPicture;
        }
    }
}


void KPObjectProperties::getTextProperties( KPObject *object )
{
    KPTextObject *obj = dynamic_cast<KPTextObject*>( object );
    if ( obj )
    {
        if ( !( m_flags & PtText ) )
        {
            m_marginsStruct = MarginsStruct( obj );
            m_protectContent = obj->isProtectContent() ? STATE_ON : STATE_OFF;

            getPenProperties( object );
            getBrushProperties( object );
            m_flags |= PtText;
        }
        else
        {
            PropValue pv = obj->isProtectContent() ? STATE_ON : STATE_OFF;
            if ( pv != m_protectContent )
            {
                m_protectContent = STATE_UNDEF;
            }
        }
    }
}
