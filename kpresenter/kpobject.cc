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
#include "kptextobject.h"

#include <qpainter.h>
#include <qwmatrix.h>
#include <qpointarray.h>
#include <qregion.h>

#include <komlParser.h>
#include <komlWriter.h>

#include <kapp.h>
#include <kdebug.h>

#include <stdlib.h>
#include <fstream.h>
#include <math.h>


/******************************************************************/
/* Class: KPObject                                                */
/******************************************************************/

/*======================== constructor ===========================*/
KPObject::KPObject()
    : orig(), ext(), shadowColor( Qt::gray ), sticky( FALSE )
{
    presNum = 0;
    disappearNum = 1;
    effect = EF_NONE;
    effect2 = EF2_NONE;
    effect3 = EF3_NONE;
    disappear = false;
    angle = 0.0;
    shadowDirection = SD_RIGHT_BOTTOM;
    shadowDistance = 0;
    dSelection = true;
    selected = false;
    presFakt = 0.0;
    zoomed = false;
    ownClipping = true;
    subPresStep = 0;
    specEffects = false;
    onlyCurrStep = true;
    inObjList = true;
    cmds = 0;
    move = false;
    dcop = 0;
}

KPObject::~KPObject()
{
}

/*================================================================*/
KPObject &KPObject::operator=( const KPObject & )
{
    return *this;
}

/*======================= get bounding rect ======================*/
QRect KPObject::getBoundingRect( int _diffx, int _diffy ) const
{
    QRect r( orig.x() - _diffx, orig.y() - _diffy,
             ext.width(), ext.height() );

    if ( shadowDistance > 0 )
    {
        int sx = r.x(), sy = r.y();
        getShadowCoords( sx, sy, shadowDirection, shadowDistance );
        QRect r2( sx, sy, r.width(), r.height() );
        r = r.unite( r2 );
    }

    if ( angle == 0.0 )
        return r;
    else
    {
        QWMatrix mtx;
        mtx.rotate( angle );
        QRect rr = mtx.map( r );

        int diffw = std::abs( rr.width() - r.width() );
        int diffh = std::abs( rr.height() - r.height() );

        return QRect( r.x() - diffw, r.y() - diffh,
                      r.width() + diffw * 2, r.height() + diffh * 2 );
    }
}

/*======================== contain point ? =======================*/
bool KPObject::contains( QPoint _point, int _diffx, int _diffy ) const
{
    if ( angle == 0.0 )
    {
        QRect r( orig.x() - _diffx, orig.y() - _diffy,
                 ext.width(), ext.height() );
        return r.contains( _point );
    }
    else
    {
        QRect br = QRect( 0, 0, ext.width(), ext.height() );
        int pw = br.width();
        int ph = br.height();
        QRect rr = br;
        int yPos = -rr.y();
        int xPos = -rr.x();
        rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

        QWMatrix m, mtx, m2;
        mtx.rotate( angle );
        m.translate( pw / 2, ph / 2 );
        m2.translate( rr.left() + xPos, rr.top() + yPos );
        m = m2 * mtx * m;

        QRect r = m.map( br );
        r.moveBy( orig.x() - _diffx, orig.y() - _diffy );

        return r.contains( _point );
    }
}

/*================================================================*/
bool KPObject::intersects( QRect _rect, int _diffx, int _diffy ) const
{
    if ( angle == 0.0 )
    {
        QRect r( orig.x() - _diffx, orig.y() - _diffy,
                 ext.width(), ext.height() );
        return r.intersects( _rect );
    }
    else
    {
        QRect br = QRect( 0, 0, ext.width(), ext.height() );
        int pw = br.width();
        int ph = br.height();
        QRect rr = br;
        int yPos = -rr.y();
        int xPos = -rr.x();
        rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

        QWMatrix m, mtx, m2;
        mtx.rotate( angle );
        m.translate( pw / 2, ph / 2 );
        m2.translate( rr.left() + xPos, rr.top() + yPos );
        m = m2 * mtx * m;

        QRect r = m.map( br );
        r.moveBy( orig.x() - _diffx, orig.y() - _diffy );

        return r.intersects( _rect );
    }
}

/*======================== get cursor ============================*/
QCursor KPObject::getCursor( QPoint _point, int _diffx, int _diffy, ModifyType &_modType ) const
{
    int px = _point.x();
    int py = _point.y();

    int ox = orig.x() - _diffx;
    int oy = orig.y() - _diffy;
    int ow = ext.width();
    int oh = ext.height();

    QRect r( ox, oy, ow, oh );

    if ( !r.contains( _point ) )
        return Qt::arrowCursor;

    if ( px >= ox && py >= oy && px <= ox + 6 && py <= oy + 6 )
    {
        _modType = MT_RESIZE_LU;
        return Qt::sizeFDiagCursor;
    }

    if ( px >= ox && py >= oy + oh / 2 - 3 && px <= ox + 6 && py <= oy + oh / 2 + 3 )
    {
        _modType = MT_RESIZE_LF;
        return Qt::sizeHorCursor;
    }

    if ( px >= ox && py >= oy + oh - 6 && px <= ox + 6 && py <= oy + oh )
    {
        _modType = MT_RESIZE_LD;
        return Qt::sizeBDiagCursor;
    }

    if ( px >= ox + ow / 2 - 3 && py >= oy && px <= ox + ow / 2 + 3 && py <= oy + 6 )
    {
        _modType = MT_RESIZE_UP;
        return Qt::sizeVerCursor;
    }

    if ( px >= ox + ow / 2 - 3 && py >= oy + oh - 6 && px <= ox + ow / 2 + 3 && py <= oy + oh )
    {
        _modType = MT_RESIZE_DN;
        return Qt::sizeVerCursor;
    }

    if ( px >= ox + ow - 6 && py >= oy && px <= ox + ow && py <= oy + 6 )
    {
        _modType = MT_RESIZE_RU;
        return Qt::sizeBDiagCursor;
    }

    if ( px >= ox + ow - 6 && py >= oy + oh / 2 - 3 && px <= ox + ow && py <= oy + oh / 2 + 3 )
    {
        _modType = MT_RESIZE_RT;
        return Qt::sizeHorCursor;
    }

    if ( px >= ox + ow - 6 && py >= oy + oh - 6 && px <= ox + ow && py <= oy + oh )
    {
        _modType = MT_RESIZE_RD;
        return Qt::sizeFDiagCursor;
    }

    _modType = MT_MOVE;
    return Qt::sizeAllCursor;
}

/*========================= zoom =================================*/
void KPObject::zoom( float _fakt )
{
    presFakt = _fakt;

    zoomed = true;

    oldOrig = orig;
    oldExt = ext;

    orig.setX( static_cast<int>( static_cast<float>( orig.x() ) * presFakt ) );
    orig.setY( static_cast<int>( static_cast<float>( orig.y() ) * presFakt ) );
    ext.setWidth( static_cast<int>( static_cast<float>( ext.width() ) * presFakt ) );
    ext.setHeight( static_cast<int>( static_cast<float>( ext.height() ) * presFakt ) );

    setSize( ext );
    setOrig( orig );
}

/*==================== zoom orig =================================*/
void KPObject::zoomOrig()
{
    ASSERT(zoomed);
    zoomed = false;

    orig = oldOrig;
    ext = oldExt;

    setSize( ext );
    setOrig( orig );
}

/*======================== draw ==================================*/
void KPObject::draw( QPainter *_painter, int _diffx, int _diffy )
{
    if ( dSelection )
    {
        _painter->save();
        _painter->translate( orig.x() - _diffx, orig.y() - _diffy );
        paintSelection( _painter );
        _painter->restore();
    }
}

/*====================== get shadow coordinates ==================*/
void KPObject::getShadowCoords( int& _x, int& _y, ShadowDirection /*_direction*/, int /*_distance*/ ) const
{
    int sx = 0, sy = 0;

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

/*======================== paint selection =======================*/
void KPObject::paintSelection( QPainter *_painter )
{
    _painter->save();
    Qt::RasterOp rop = _painter->rasterOp();

    _painter->setRasterOp( Qt::NotROP );

    if ( getType() == OT_TEXT && dynamic_cast<KPTextObject*>( this )->getDrawEditRect() )
    {
        _painter->save();

        if ( angle != 0 )
        {
            QRect br = QRect( 0, 0, ext.width(), ext.height() );
            int pw = br.width();
            int ph = br.height();
            QRect rr = br;
            int yPos = -rr.y();
            int xPos = -rr.x();
            rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

            QWMatrix m, mtx, m2;
            mtx.rotate( angle );
            m.translate( pw / 2, ph / 2 );
            m2.translate( rr.left() + xPos, rr.top() + yPos );
            m = m2 * mtx * m;

            _painter->setWorldMatrix( m, true );
        }

        _painter->setPen( QPen( Qt::black, 1, Qt::DotLine ) );
        _painter->setBrush( Qt::NoBrush );
        _painter->drawRect( 0, 0, ext.width(), ext.height() );

        _painter->restore();
    }

    _painter->setPen( QPen( Qt::black, 1, Qt::SolidLine ) );
    _painter->setBrush( Qt::black );

    if ( selected )
    {
        _painter->fillRect( 0, 0, 6, 6, Qt::black );
        _painter->fillRect( 0, ext.height() / 2 - 3, 6, 6, Qt::black );
        _painter->fillRect( 0, ext.height() - 6, 6, 6, Qt::black );
        _painter->fillRect( ext.width() - 6, 0, 6, 6, Qt::black );
        _painter->fillRect( ext.width() - 6, ext.height() / 2 - 3, 6, 6, Qt::black );
        _painter->fillRect( ext.width() - 6, ext.height() - 6, 6, 6, Qt::black );
        _painter->fillRect( ext.width() / 2 - 3, 0, 6, 6, Qt::black );
        _painter->fillRect( ext.width() / 2 - 3, ext.height() - 6, 6, 6, Qt::black );
    }

    _painter->setRasterOp( rop );
    _painter->restore();
}

/*======================== do delete =============================*/
void KPObject::doDelete()
{
    if ( cmds == 0 && !inObjList )delete this;
}

/*=============================================================*/
DCOPObject* KPObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPresenterObjectIface( this );

    return dcop;
}

void KPObject::setupClipRegion( QPainter *painter, const QRegion &clipRegion )
{
    QRegion region = painter->clipRegion();
    if ( region.isEmpty() )
        region = clipRegion;
    else
        region.unite( clipRegion );

    painter->setClipRegion( region );
}
