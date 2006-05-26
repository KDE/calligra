// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2004 Thorsten Zachmann <zachmann@kde.org>

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

#include "KPrEffectHandler.h"

#include "KPrObject.h"
#include "KPrDocument.h"
#include "KPrView.h"
#include "KPrCanvas.h"

#include <KoZoomHandler.h>
#include <kapplication.h>

#include <qpainter.h>
#include <qpaintdevice.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3PtrList>

#include <unistd.h>


KPrEffectHandler::KPrEffectHandler( PresStep step, bool back, QPaintDevice *dst, QPixmap *src,
                              const Q3PtrList<KPrObject> &objects, KPrView *view, int _presSpeed )
: m_effectStep(0), m_step(step), m_back(back), m_dst(dst),
  m_paint(*src), m_objects(objects), m_view(view), m_soundEffect(QString::null),
  m_objectTimer(1)
{
    m_src = new QPixmap( *src );

    Q3PtrListIterator<KPrObject> it( m_objects );
    for ( ; it.current(); ++it )
    {
        KPrObject *object = it.current();
        if ( object->getAppearStep() == m_step.m_step
          && ( m_step.m_subStep == 0
            || ( object->getType() == OT_TEXT
              && object->getEffect2() == EF2T_PARA ) ) )
        {
            m_appearEffectObjects.append( object );
            if ( object->getAppearSoundEffect() )
            {
                m_soundEffect = object->getAppearSoundEffectFileName();
            }

            if ( object->getAppearTimer() > m_objectTimer )
            {
                m_objectTimer = object->getAppearTimer();
            }
        }
        else if ( object->getDisappear() && object->getDisappearStep() == m_step.m_step )
        {
            m_disappearEffectObjects.append( object );
            if ( object->getDisappearSoundEffect() )
            {
                m_soundEffect = object->getDisappearSoundEffectFileName();
            }

            if ( object->getDisappearTimer() > m_objectTimer )
            {
                m_objectTimer = object->getDisappearTimer();
            }
        }
    }

    float speedFactor = 150.0 / static_cast<float>( _presSpeed + 2 );
    m_stepWidth = static_cast<int>( m_src->width() / speedFactor );
    m_stepHeight = static_cast<int>( m_src->height() / speedFactor );
    m_lastRepaintRects.setAutoDelete( true );
}


KPrEffectHandler::~KPrEffectHandler()
{
    delete m_src;
}


bool KPrEffectHandler::doEffect()
{
    if ( m_effectStep == 0 )
    {
        if ( !m_view->kPresenterDoc()->spManualSwitch() )
        {
            m_view->stopAutoPresTimer();
        }

        if ( !m_soundEffect.isNull() )
        {
            m_view->getCanvas()->stopSound();
            m_view->getCanvas()->playSound( m_soundEffect );
        }
    }
    if ( !m_appearEffectObjects.isEmpty() || !m_disappearEffectObjects.isEmpty() )
    {
        Q3PtrList<QRect> m_removeRects;
        KPrObject *appearObject = m_appearEffectObjects.first();
        KPrObject *disappearObject = m_disappearEffectObjects.first();
        Q3PtrListIterator<KPrObject> it( m_objects );
        KPrObject *object;

        while ( ( object = it.current() ) != 0 )
        {
            int speed[] = { 50, 35, 20 };
            
            ++it;
            if ( object == appearObject )
            {
                bool next = true;

                double speedFactor = speed[object->getAppearSpeed()];
                
                m_stepWidth = static_cast<int>( m_src->width() / speedFactor );
                m_stepHeight = static_cast<int>( m_src->height() / speedFactor );

                if ( doAppearEffectStep( object ) )
                {
                    m_removeRects.append( m_repaintRects.getLast() );
                    if ( appearObject != m_appearEffectObjects.getLast() )
                    {
                        next = false;
                    }
                    m_appearEffectObjects.remove();

                    // add object to src
                    drawObject( object, 0, 0, m_src );
                }
                if ( next )
                    appearObject = m_appearEffectObjects.next();
                else
                    appearObject = m_appearEffectObjects.current();
            }
            else if ( object == disappearObject )
            {
                bool next = true;

                double speedFactor = speed[object->getDisappearSpeed()];
                
                m_stepWidth = static_cast<int>( m_src->width() / speedFactor );
                m_stepHeight = static_cast<int>( m_src->height() / speedFactor );

                if ( doDisappearEffectStep( object ) )
                {
                    if ( disappearObject != m_disappearEffectObjects.getLast() )
                    {
                        next = false;
                    }
                    m_disappearEffectObjects.remove();
                }

                if ( next )
                    disappearObject = m_disappearEffectObjects.next();
                else
                    disappearObject = m_disappearEffectObjects.current();
            }
        }

        ++m_effectStep;
        QRect *r;
#if 0 // used for debugging effects
        Q3PtrListIterator<QRect> it_debug(m_repaintRects);
        while( ( r = it_debug.current() ) != 0 )
        {
            ++it_debug;
            QPainter p( &m_paint );
            QPen pen = QPen( Qt::red, 1, Qt::DashLine );
            p.setPen( pen );
            p.drawRect( *r );
        }

        Q3PtrListIterator<QRect> it_debug2(m_lastRepaintRects);
        while( ( r = it_debug2.current() ) != 0 )
        {
            ++it_debug2;
            QPainter p( &m_paint );
            QPen pen = QPen( Qt::green, 1, Qt::DashLine );
            p.setPen( pen );
            p.drawRect( *r );
        }
        sleep( 1 );
#endif

        Q3PtrListIterator<QRect> it_r(m_repaintRects);
#if 1
        while( ( r = it_r.current() ) != 0 )
        {
            ++it_r;
            //bitBlt( m_dst, r->x(), r->y(), &m_paint, r->x(), r->y(), r->width(), r->height() );
            bitBlt( m_dst, r->x() - 5, r->y() - 5, &m_paint, r->x() - 5, r->y() - 5, r->width() + 10, r->height() + 10 );
        }
        Q3PtrListIterator<QRect> it2(m_lastRepaintRects);
        while( ( r = it2.current() ) != 0 )
        {
            ++it2;
            //bitBlt( m_dst, r->x(), r->y(), &m_paint, r->x(), r->y(), r->width(), r->height() );
            bitBlt( m_dst, r->x() - 5, r->y() - 5, &m_paint, r->x() - 5, r->y() - 5, r->width() + 10, r->height() + 10 );
        }
#else
        bitBlt( m_dst, 0, 0, &m_paint);
#endif

        m_lastRepaintRects = m_repaintRects;
        Q3PtrListIterator<QRect> it3(m_removeRects);
        while( ( r = it3.current() ) != 0 )
        {
            ++it3;
            m_lastRepaintRects.remove( r );
        }

        m_repaintRects.clear();

        m_paint = *m_src;

    }
    bool retval = ( m_appearEffectObjects.isEmpty() && m_disappearEffectObjects.isEmpty() );

    if ( retval && !m_view->kPresenterDoc()->spManualSwitch() )
    {
        m_view->setAutoPresTimer( m_objectTimer );
    }
    return retval;
}


void KPrEffectHandler::finish()
{
    KPrObject *appearObject = m_appearEffectObjects.first();
    KPrObject *disappearObject = m_disappearEffectObjects.first();
    Q3PtrListIterator<KPrObject> it( m_objects );
    KPrObject *object;

    while ( ( object = it.current() ) != 0 )
    {
        ++it;
        if ( object == appearObject )
        {
            bool next = true;

            QRect objectRect = m_view->zoomHandler()->zoomRect( object->getRealRect() );
            m_repaintRects.append( new QRect( objectRect ) );

            if ( appearObject != m_appearEffectObjects.getLast() )
            {
                next = false;
            }
            m_appearEffectObjects.remove();

            // add object to src
            drawObject( object, 0, 0, m_src );

            if ( next )
                appearObject = m_appearEffectObjects.next();
            else
                appearObject = m_appearEffectObjects.current();
        }
        else if ( object == disappearObject )
        {
            bool next = true;

            if ( disappearObject != m_disappearEffectObjects.getLast() )
            {
                next = false;
            }
            m_disappearEffectObjects.remove();

            if ( next )
                disappearObject = m_disappearEffectObjects.next();
            else
                disappearObject = m_disappearEffectObjects.current();
        }
    }
    bitBlt( m_dst, 0, 0, m_src );

    if ( !m_view->kPresenterDoc()->spManualSwitch() && m_objectTimer > 0 )
    {
        m_view->setAutoPresTimer( m_objectTimer );
    }
}


bool KPrEffectHandler::doAppearEffectStep( KPrObject *object )
{
    bool positionReached = true;

    switch( object->getEffect() )
    {
        case EF_NONE:
            positionReached = appearNone( object );
            break;
        case EF_COME_LEFT:
            positionReached = appearComeLeft( object );
            break;
        case EF_COME_TOP:
            positionReached = appearComeTop( object );
            break;
        case EF_COME_RIGHT:
            positionReached = appearComeRight( object );
            break;
        case EF_COME_BOTTOM:
            positionReached = appearComeBottom( object );
            break;
        case EF_COME_LEFT_TOP:
            positionReached = appearComeLeftTop( object );
            break;
        case EF_COME_LEFT_BOTTOM:
            positionReached = appearComeLeftBottom( object );
            break;
        case EF_COME_RIGHT_TOP:
            positionReached = appearComeRightTop( object );
            break;
        case EF_COME_RIGHT_BOTTOM:
            positionReached = appearComeRightBottom( object );
            break;
        case EF_WIPE_LEFT:
            positionReached = appearWipeLeft( object );
            break;
        case EF_WIPE_RIGHT:
            positionReached = appearWipeRight( object );
            break;
        case EF_WIPE_TOP:
            positionReached = appearWipeTop( object );
            break;
        case EF_WIPE_BOTTOM:
            positionReached = appearWipeBottom( object );
            break;
        default:
            break;
    }
    return positionReached;
}


bool KPrEffectHandler::doDisappearEffectStep( KPrObject *object )
{
    bool positionReached = true;

    switch( object->getEffect3() )
    {
        case EF3_NONE:
            break;
        case EF3_GO_LEFT:
            positionReached = disappearGoLeft( object );
            break;
        case EF3_GO_TOP:
            positionReached = disappearGoTop( object );
            break;
        case EF3_GO_RIGHT:
            positionReached = disappearGoRight( object );
            break;
        case EF3_GO_BOTTOM:
            positionReached = disappearGoBottom( object );
            break;
        case EF3_GO_LEFT_TOP:
            positionReached = disappearGoLeftTop( object );
            break;
        case EF3_GO_LEFT_BOTTOM:
            positionReached = disappearGoLeftBottom( object );
            break;
        case EF3_GO_RIGHT_TOP:
            positionReached = disappearGoRightTop( object );
            break;
        case EF3_GO_RIGHT_BOTTOM:
            positionReached = disappearGoRightBottom( object );
            break;
        case EF3_WIPE_LEFT:
            positionReached = disappearWipeLeft( object );
            break;
        case EF3_WIPE_RIGHT:
            positionReached = disappearWipeRight( object );
            break;
        case EF3_WIPE_TOP:
            positionReached = disappearWipeTop( object );
            break;
        case EF3_WIPE_BOTTOM:
            positionReached = disappearWipeBottom( object );
            break;
        default:
            break;
    }
    return positionReached;
}

bool KPrEffectHandler::appearNone( KPrObject *object )
{
    QRect objectRect = m_view->zoomHandler()->zoomRect( object->getRealRect() );
    m_repaintRects.append( new QRect( objectRect ) );
    drawObject( object, 0, 0, &m_paint );
    return true;
}


bool KPrEffectHandler::appearComeLeft( KPrObject *object )
{
    QRect objectRect = m_view->zoomHandler()->zoomRect( object->getRealRect() );
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int ox = objectRect.x();
    int xdiff = repaintRect.x() - ox;

    bool positionReached = false;
    int x = m_effectStep * m_stepWidth - objectRect.width();
    if ( x >= ox )
    {
        x = ox;
        positionReached = true;
    }
    repaintRect.moveLeft( x + xdiff );
    m_repaintRects.append( new QRect( repaintRect ) );

    //kDebug(33001) << "KPrEffectHandler appearComeLeft x = " << x << " " << positionReached << endl;
    drawObject( object, x - ox, 0, &m_paint );

    return positionReached;
}


bool KPrEffectHandler::appearComeTop( KPrObject *object )
{
    QRect objectRect = m_view->zoomHandler()->zoomRect( object->getRealRect() );
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int oy = objectRect.y();
    int ydiff = repaintRect.y() - oy;

    bool positionReached = false;
    int y = m_effectStep * m_stepHeight - objectRect.height();
    if ( y >= oy )
    {
        y = oy;
        positionReached = true;
    }
    repaintRect.moveTop( y + ydiff );
    m_repaintRects.append( new QRect( repaintRect ) );

    //kDebug(33001) << "KPrEffectHandler appearComeTop y = " << y << " " << positionReached << endl;
    drawObject( object, 0, y - oy, &m_paint );

    return positionReached;
}


bool KPrEffectHandler::appearComeRight( KPrObject *object )
{
    QRect objectRect = m_view->zoomHandler()->zoomRect( object->getRealRect() );
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int ox = objectRect.x();
    int xdiff = repaintRect.x() - ox;

    bool positionReached = false;
    int x = m_src->width() - m_effectStep * m_stepWidth;
    if ( x <= ox )
    {
        x = ox;
        positionReached = true;
    }
    repaintRect.moveLeft( x + xdiff );
    m_repaintRects.append( new QRect( repaintRect ) );

    //kDebug(33001) << "KPrEffectHandler appearComeRight x = " << x << " " << positionReached << endl;
    drawObject( object, x - ox, 0, &m_paint );

    return positionReached;
}


bool KPrEffectHandler::appearComeBottom( KPrObject *object )
{
    QRect objectRect = m_view->zoomHandler()->zoomRect( object->getRealRect() );
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int oy = objectRect.y();
    int ydiff = repaintRect.y() - oy;

    bool positionReached = false;
    int y = m_src->height() - m_effectStep * m_stepHeight;
    if ( y <= oy )
    {
        y = oy;
        positionReached = true;
    }
    repaintRect.moveTop( y + ydiff );
    m_repaintRects.append( new QRect( repaintRect ) );

    //kDebug(33001) << "KPrEffectHandler appearComeBottom y = " << y << " " << positionReached << endl;
    drawObject( object, 0, y - oy, &m_paint );

    return positionReached;
}


bool KPrEffectHandler::appearComeLeftTop( KPrObject *object )
{
    QRect objectRect = m_view->zoomHandler()->zoomRect( object->getRealRect() );
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int ox = objectRect.x();
    int oy = objectRect.y();
    int xdiff = repaintRect.x() - ox;
    int ydiff = repaintRect.y() - oy;

    bool positionReached = false;
    int x = m_effectStep * m_stepWidth - objectRect.width();
    int y = m_effectStep * m_stepHeight - objectRect.height();

    if ( x >= ox )
        x = ox;

    if ( y >= oy )
        y = oy;

    if ( x == ox && y == oy )
        positionReached = true;

    repaintRect.moveLeft( x + xdiff );
    repaintRect.moveTop( y + ydiff );
    m_repaintRects.append( new QRect( repaintRect ) );

    //kDebug(33001) << "KPrEffectHandler appearComeLeftTop x = " << x << ",y = " << y << " " <<  positionReached << endl;
    drawObject( object, x - ox, y - oy, &m_paint );

    return positionReached;
}


bool KPrEffectHandler::appearComeLeftBottom( KPrObject *object )
{
    QRect objectRect = m_view->zoomHandler()->zoomRect( object->getRealRect() );
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int ox = objectRect.x();
    int oy = objectRect.y();
    int xdiff = repaintRect.x() - ox;
    int ydiff = repaintRect.y() - oy;

    bool positionReached = false;
    int x = m_effectStep * m_stepWidth - objectRect.width();
    int y = m_src->height() - m_effectStep * m_stepHeight;

    if ( x >= ox )
        x = ox;

    if ( y <= oy )
        y = oy;

    if ( x == ox && y == oy )
        positionReached = true;

    repaintRect.moveLeft( x + xdiff );
    repaintRect.moveTop( y + ydiff );
    m_repaintRects.append( new QRect( repaintRect ) );

    //kDebug(33001) << "KPrEffectHandler appearComeLeftBottom x = " << x << ",y = " << y << " " <<  positionReached << endl;
    drawObject( object, x - ox, y - oy, &m_paint );

    return positionReached;
}


bool KPrEffectHandler::appearComeRightTop( KPrObject *object )
{
    QRect objectRect = m_view->zoomHandler()->zoomRect( object->getRealRect() );
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int ox = objectRect.x();
    int oy = objectRect.y();
    int xdiff = repaintRect.x() - ox;
    int ydiff = repaintRect.y() - oy;

    bool positionReached = false;
    int x = m_src->width() - m_effectStep * m_stepWidth;
    int y = m_effectStep * m_stepHeight - objectRect.height();

    if ( x <= ox )
        x = ox;

    if ( y >= oy )
        y = oy;

    if ( x == ox && y == oy )
        positionReached = true;

    repaintRect.moveLeft( x + xdiff );
    repaintRect.moveTop( y + ydiff );
    m_repaintRects.append( new QRect( repaintRect ) );

    //kDebug(33001) << "KPrEffectHandler appearComeRightTop x = " << x << ",y = " << y << " " <<  positionReached << endl;
    drawObject( object, x - ox, y - oy, &m_paint );

    return positionReached;
}


bool KPrEffectHandler::appearComeRightBottom( KPrObject *object )
{
    QRect objectRect = m_view->zoomHandler()->zoomRect( object->getRealRect() );
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int ox = objectRect.x();
    int oy = objectRect.y();
    int xdiff = repaintRect.x() - ox;
    int ydiff = repaintRect.y() - oy;

    bool positionReached = false;
    int x = m_src->width() - m_effectStep * m_stepWidth;
    int y = m_src->height() - m_effectStep * m_stepHeight;

    if ( x <= ox )
        x = ox;

    if ( y <= oy )
        y = oy;

    if ( x == ox && y == oy )
        positionReached = true;

    repaintRect.moveLeft( x + xdiff );
    repaintRect.moveTop( y + ydiff );
    m_repaintRects.append( new QRect( repaintRect ) );

    //kDebug(33001) << "KPrEffectHandler appearComeRightBottom x = " << x << ",y = " << y << " " <<  positionReached << endl;
    drawObject( object, x - ox, y - oy, &m_paint );

    return positionReached;
}


bool KPrEffectHandler::appearWipeLeft( KPrObject *object )
{
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int ow = repaintRect.width();

    bool positionReached = false;
    int w = m_effectStep * m_stepWidth;
    if ( w >= ow )
    {
        w = ow;
        positionReached = true;
    }

    repaintRect.setWidth( w );
    m_repaintRects.append( new QRect( repaintRect ) );
    drawObject( object, 0, 0, &m_paint, &repaintRect );
    //kDebug(33001) << "KPrEffectHandler appearWipeLeft w = " << w << positionReached << endl;
    return positionReached;
}

bool KPrEffectHandler::appearWipeRight( KPrObject *object )
{
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int ow = repaintRect.width();

    bool positionReached = false;
    int w = m_effectStep * m_stepWidth;
    if ( w >= ow )
    {
        w = ow;
        positionReached = true;
    }

    repaintRect.setLeft( repaintRect.right() - w );
    m_repaintRects.append( new QRect( repaintRect ) );
    drawObject( object, 0, 0, &m_paint, &repaintRect );
    //kDebug(33001) << "KPrEffectHandler appearWipeLeft w = " << w << positionReached << endl;
    return positionReached;
}


bool KPrEffectHandler::appearWipeTop( KPrObject *object )
{
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int oh = repaintRect.height();

    bool positionReached = false;
    int h = m_effectStep * m_stepHeight;
    if ( h >= oh )
    {
        h = oh;
        positionReached = true;
    }

    repaintRect.setHeight( h );
    m_repaintRects.append( new QRect( repaintRect ) );
    drawObject( object, 0, 0, &m_paint, &repaintRect );
    //kDebug(33001) << "KPrEffectHandler appearWipeLeft h = " << h << positionReached << endl;
    return positionReached;
}


bool KPrEffectHandler::appearWipeBottom( KPrObject *object )
{
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int oh = repaintRect.height();

    bool positionReached = false;
    int h = m_effectStep * m_stepHeight;
    if ( h >= oh )
    {
        h = oh;
        positionReached = true;
    }

    repaintRect.setTop( repaintRect.bottom() - h );
    m_repaintRects.append( new QRect( repaintRect ) );
    drawObject( object, 0, 0, &m_paint, &repaintRect );
    //kDebug(33001) << "KPrEffectHandler appearWipeLeft h = " << h << positionReached << endl;
    return positionReached;
}


bool KPrEffectHandler::disappearGoLeft( KPrObject *object )
{
    QRect objectRect = m_view->zoomHandler()->zoomRect( object->getRealRect() );
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int ox = objectRect.x();

    bool positionReached = false;
    int x = m_effectStep * m_stepWidth;
    if ( ox - x <= -objectRect.width() )
    {
        positionReached = true;
    }
    else
    {
        repaintRect.moveBy( -x, 0 );
        m_repaintRects.append( new QRect( repaintRect ) );
        //kDebug(33001) << "KPrEffectHandler disappearGoLeft x = " << -x + ox << " " << positionReached << endl;
        drawObject( object, -x, 0, &m_paint );
    }

    return positionReached;
}


bool KPrEffectHandler::disappearGoTop( KPrObject *object )
{
    QRect objectRect = m_view->zoomHandler()->zoomRect( object->getRealRect() );
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int oy = objectRect.y();

    bool positionReached = false;
    int y = m_effectStep * m_stepHeight;
    if ( oy - y <= -objectRect.height() )
    {
        positionReached = true;
    }
    else
    {
        repaintRect.moveBy( 0, -y );
        m_repaintRects.append( new QRect( repaintRect ) );
        //kDebug(33001) << "KPrEffectHandler disappearGoTop y = " << -y + oy << " " << positionReached << endl;
        drawObject( object, 0, -y, &m_paint );
    }

    return positionReached;
}


bool KPrEffectHandler::disappearGoRight( KPrObject *object )
{
    int ox = m_view->zoomHandler()->zoomItX( object->getRealRect().x() );
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );

    bool positionReached = false;
    int x = m_effectStep * m_stepWidth;
    if ( x + ox >= m_src->width() )
    {
        positionReached = true;
    }
    else
    {
        repaintRect.moveBy( x, 0 );
        m_repaintRects.append( new QRect( repaintRect ) );
        //kDebug(33001) << "KPrEffectHandler disappearGoRight x = " << x + ox << " " << positionReached << endl;
        drawObject( object, x, 0, &m_paint );
    }

    return positionReached;
}


bool KPrEffectHandler::disappearGoBottom( KPrObject *object )
{
    int oy = m_view->zoomHandler()->zoomItY( object->getRealRect().y() );
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );

    bool positionReached = false;
    int y = m_effectStep * m_stepHeight;
    if ( oy + y >= m_src->height() )
    {
        positionReached = true;
    }
    else
    {
        repaintRect.moveBy( 0, y );
        m_repaintRects.append( new QRect( repaintRect ) );
        //kDebug(33001) << "KPrEffectHandler disappearGoBottom y = " << y + oy << " " << positionReached << endl;
        drawObject( object, 0, y, &m_paint );
    }

    return positionReached;
}


bool KPrEffectHandler::disappearGoLeftTop( KPrObject *object )
{
    QRect objectRect = m_view->zoomHandler()->zoomRect( object->getRealRect() );
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int ox = objectRect.x();
    int oy = objectRect.y();

    bool positionReached = false;
    int x = m_effectStep * m_stepWidth;
    int y = m_effectStep * m_stepHeight;
    if ( ox - x <= -objectRect.width() || oy - y <= -objectRect.height() )
    {
        positionReached = true;
    }
    else
    {
        repaintRect.moveBy( -x, -y );
        m_repaintRects.append( new QRect( repaintRect ) );
        //kDebug(33001) << "KPrEffectHandler disappearGoLeftTop x = " << -x + ox << ", y = " << -y + oy << positionReached << endl;
        drawObject( object, -x, -y, &m_paint );
    }

    return positionReached;
}


bool KPrEffectHandler::disappearGoLeftBottom( KPrObject *object )
{
    QRect objectRect = m_view->zoomHandler()->zoomRect( object->getRealRect() );
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int ox = objectRect.x();
    int oy = objectRect.y();

    bool positionReached = false;
    int x = m_effectStep * m_stepWidth;
    int y = m_effectStep * m_stepHeight;
    if ( ox - x <= -objectRect.width() || oy + y >= m_src->height() )
    {
        positionReached = true;
    }
    else
    {
        repaintRect.moveBy( -x, y );
        m_repaintRects.append( new QRect( repaintRect ) );
        //kDebug(33001) << "KPrEffectHandler disappearGoLeftTop x = " << -x + ox << ", y = " << y + oy << positionReached << endl;
        drawObject( object, -x, y, &m_paint );
    }

    return positionReached;
}


bool KPrEffectHandler::disappearGoRightTop( KPrObject *object )
{
    QRect objectRect = m_view->zoomHandler()->zoomRect( object->getRealRect() );
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int ox = objectRect.x();
    int oy = objectRect.y();

    bool positionReached = false;
    int x = m_effectStep * m_stepWidth;
    int y = m_effectStep * m_stepHeight;
    if ( ox + x >= m_src->width() || oy - y <= -objectRect.height() )
    {
        positionReached = true;
    }
    else
    {
        repaintRect.moveBy( x, -y );
        m_repaintRects.append( new QRect( repaintRect ) );
        //kDebug(33001) << "KPrEffectHandler disappearGoLeftTop x = " << x + ox << ", y = " << -y + oy << positionReached << endl;
        drawObject( object, x, -y, &m_paint );
    }

    return positionReached;
}


bool KPrEffectHandler::disappearGoRightBottom( KPrObject *object )
{
    QRect objectRect = m_view->zoomHandler()->zoomRect( object->getRealRect() );
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int ox = objectRect.x();
    int oy = objectRect.y();

    bool positionReached = false;
    int x = m_effectStep * m_stepWidth;
    int y = m_effectStep * m_stepHeight;
    if ( ox + x >= m_src->width() || oy + y >= m_src->height() )
    {
        positionReached = true;
    }
    else
    {
        repaintRect.moveBy( x, y );
        m_repaintRects.append( new QRect( repaintRect ) );
        //kDebug(33001) << "KPrEffectHandler disappearGoLeftTop x = " << x + ox << ", y = " << y + oy << positionReached << endl;
        drawObject( object, x, y, &m_paint );
    }

    return positionReached;
}


bool KPrEffectHandler::disappearWipeLeft( KPrObject *object )
{
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int ow = repaintRect.width();

    bool positionReached = false;
    int w = ow - m_effectStep * m_stepWidth;
    if ( w <= 0 )
    {
        positionReached = true;
    }
    else
    {
        repaintRect.setWidth( w );
        m_repaintRects.append( new QRect( repaintRect ) );
        drawObject( object, 0, 0, &m_paint, &repaintRect );
        //kDebug(33001) << "KPrEffectHandler appearWipeLeft w = " << w << positionReached << endl;
    }
    return positionReached;
}


bool KPrEffectHandler::disappearWipeRight( KPrObject *object )
{
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int ow = repaintRect.width();

    bool positionReached = false;
    int w = ow - m_effectStep * m_stepWidth;
    if ( w <= 0 )
    {
        positionReached = true;
    }
    else
    {
        repaintRect.setLeft( repaintRect.right() - w );
        m_repaintRects.append( new QRect( repaintRect ) );
        drawObject( object, 0, 0, &m_paint, &repaintRect );
        //kDebug(33001) << "KPrEffectHandler appearWipeLeft w = " << w << positionReached << endl;
    }
    return positionReached;
}


bool KPrEffectHandler::disappearWipeTop( KPrObject *object )
{
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int oh = repaintRect.height();

    bool positionReached = false;
    int h = oh - m_effectStep * m_stepHeight;
    if ( h <= 0 )
    {
        positionReached = true;
    }
    else
    {
        repaintRect.setHeight( h );
        m_repaintRects.append( new QRect( repaintRect ) );
        drawObject( object, 0, 0, &m_paint, &repaintRect );
        //kDebug(33001) << "KPrEffectHandler appearWipeLeft h = " << h << positionReached << endl;
    }
    return positionReached;
}


bool KPrEffectHandler::disappearWipeBottom( KPrObject *object )
{
    QRect repaintRect = m_view->zoomHandler()->zoomRect( object->getRepaintRect() );
    int oh = repaintRect.height();

    bool positionReached = false;
    int h = oh - m_effectStep * m_stepHeight;
    if ( h <= 0 )
    {
        positionReached = true;
    }
    else
    {
        repaintRect.setTop( repaintRect.bottom() - h );
        m_repaintRects.append( new QRect( repaintRect ) );
        drawObject( object, 0, 0, &m_paint, &repaintRect );
        //kDebug(33001) << "KPrEffectHandler appearWipeLeft h = " << h << positionReached << endl;
    }
    return positionReached;
}


void KPrEffectHandler::drawObject( KPrObject *object, int x, int y, QPixmap *screen, QRect *clipRect )
{
    QPainter p;
    p.begin( screen );
    if ( clipRect )
    {
        p.setClipRect( *clipRect, QPainter::CoordPainter );
    }

    p.translate( x, y );

    if ( object->getAppearStep() == m_step.m_step && ! m_back )
    {
        object->setSubPresStep( m_step.m_subStep );
        object->doSpecificEffects( true );
    }

    object->draw( &p, m_view->zoomHandler(), m_step.m_pageNumber, SM_NONE, false );

    if ( object->getAppearStep() == m_step.m_step && ! m_back )
    {
        object->setSubPresStep( 0 );
        object->doSpecificEffects( false );
    }

    p.translate( -x, -y );

    // redraw objects which lie above the currenty drawn
    m_objects.findRef( object );
    KPrObject *obj;
    while ( ( obj = m_objects.next() ) != 0 )
    {
        if ( ( obj->getAppearStep() < m_step.m_step
            || obj->getAppearStep() == m_step.m_step && !m_appearEffectObjects.containsRef( obj ) )
            && ( ( obj->getDisappear() && obj->getDisappearStep() > m_step.m_step ) || ! obj->getDisappear() )
            && m_view->zoomHandler()->zoomRect( obj->getRealRect()).intersects(*m_repaintRects.getLast()) )
        {
            if ( obj->getAppearStep() == m_step.m_step && ! m_back )
            {
                obj->setSubPresStep( m_step.m_subStep );
                obj->doSpecificEffects( true );
            }
            obj->draw( &p, m_view->zoomHandler(), m_step.m_pageNumber, SM_NONE, false );
            if ( obj->getAppearStep() == m_step.m_step && ! m_back )
            {
                obj->setSubPresStep( 0 );
                obj->doSpecificEffects( false );
            }
        }
    }

    p.end();
}
