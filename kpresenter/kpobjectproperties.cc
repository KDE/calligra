// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

   The code is mostly a copy from kword/framedia.cc

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

#include "kpobjectproperties.h"

#include "kpobject.h"
#include "kpcubicbeziercurveobject.h"
#include "kpgroupobject.h"
#include "kprectobject.h"
#include "kppolygonobject.h"
#include "kppieobject.h"
#include "kppixmapobject.h"
#include "kppointobject.h"
#include "kpquadricbeziercurveobject.h"
#include "kptextobject.h"

KPObjectProperties::KPObjectProperties( const QPtrList<KPObject> &objects )
: m_objects( objects )
, m_flags( 0 )
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
                m_flags |= PtPen | PtLineEnds | PtOther;
                break;
            case OT_FREEHAND:
            case OT_POLYLINE:
                m_flags |= PtPen | PtOther;
                if( !static_cast<KPPointObject*>( it.current() )->isClosed() )
                    m_flags |= PtLineEnds;
                break;
            case OT_QUADRICBEZIERCURVE:
                m_flags |= PtPen | PtOther;
                if ( !static_cast<KPQuadricBezierCurveObject*>( it.current() )->isClosed() )
                    m_flags |= PtLineEnds;
                break;
            case OT_CUBICBEZIERCURVE:
                m_flags |= PtPen | PtOther;
                if ( !static_cast<KPCubicBezierCurveObject*>( it.current() )->isClosed() )
                    m_flags |= PtLineEnds;
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
                m_flags |= PtPen | PtBrush | PtOther;
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


void KPObjectProperties::getRectProperties( KPObject *object )
{
    if ( !( m_flags & PtRectangle ) )
    {
        KPRectObject *obj = dynamic_cast<KPRectObject*>( object );
        if ( obj )
        {
            obj->getRnds( m_rectValues.xRnd, m_rectValues.yRnd );

            m_flags |= PtPen | PtBrush | PtRectangle;
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
            m_flags |= PtPen | PtBrush | PtPolygon;
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

            m_flags |= PtPen | PtPie;
            if ( obj->getPieType() != PT_ARC )
                m_flags |= PtBrush;
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
            m_flags |= PtPen | PtBrush | PtPicture;
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
        }
        else
        {
            PropValue pv = obj->isProtectContent() ? STATE_ON : STATE_OFF;
            if ( pv != m_protectContent )
            {
                m_protectContent = STATE_UNDEF;
            }
        }
        m_flags |= PtPen | PtBrush | PtText;
    }
}
