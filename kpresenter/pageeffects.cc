// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2002 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2002 Harri Porten <porten@kde.org>
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "pageeffects.h"

#include <qpixmap.h>
#include <qwidget.h>

#include <kapplication.h>
#include <kdebug.h>


KPPageEffects::KPPageEffects( QPaintDevice *dst, const QPixmap &pageTo, PageEffect effect, PresSpeed speed )
: m_dst( dst ), m_pageTo( pageTo ), m_pageFrom(m_pageTo.width(),m_pageTo.height()), m_effect(effect), m_speed(speed), m_effectStep(0)
, m_width(m_pageTo.width()), m_height(m_pageTo.height()), m_finished(false)
{
}


KPPageEffects::~KPPageEffects()
{
}


bool KPPageEffects::doEffect()
{
    if ( !m_finished )
    {
        switch ( m_effect )
        {
            case PEF_NONE:
                m_finished = effectNone();
                break;
            case PEF_CLOSE_HORZ:
                m_finished = effectCloseHorizontal();
                break;
            case PEF_CLOSE_VERT:
                m_finished = effectCloseVertical();
                break;
            case PEF_CLOSE_ALL:
                m_finished = effectCloseFromAllDirections();
                break;
            case PEF_OPEN_HORZ:
                m_finished = effectOpenHorizontal();
                break;
            case PEF_OPEN_VERT:
                m_finished = effectOpenVertical();
                break;
            case PEF_OPEN_ALL:
                m_finished = effectOpenFromAllDirections();
                break;
            case PEF_INTERLOCKING_HORZ_1:
                m_finished = effectInterlockingHorizontal1();
                break;
            case PEF_INTERLOCKING_HORZ_2:
                m_finished = effectInterlockingHorizontal2();
                break;
            case PEF_INTERLOCKING_VERT_1:
                m_finished = effectInterlockingVertical1();
                break;
            case PEF_INTERLOCKING_VERT_2:
                m_finished = effectInterlockingVertical2();
                break;
            case PEF_SURROUND1:
                m_finished = effectSurround1();
                break;
            case PEF_FLY1:
                m_finished = effectFlyAway1();
                break;
            case PEF_BLINDS_HOR:
                m_finished = effectBlindsHorizontal();
                break;
            case PEF_BLINDS_VER:
                m_finished = effectBlindsVertical();
                break;
            case PEF_BOX_IN:
                m_finished = effectBoxIn();
                break;
            case PEF_BOX_OUT:
                m_finished = effectBoxOut();
                break;
            case PEF_CHECKBOARD_ACROSS:
                m_finished = effectCheckboardAcross();
                break;
            case PEF_CHECKBOARD_DOWN:
                m_finished = effectCheckboardDown();
                break;
            case PEF_COVER_DOWN:
                m_finished = effectCoverDown();
                break;
            case PEF_UNCOVER_DOWN:
                m_finished = effectUncoverDown();
                break;
            case PEF_COVER_UP:
                m_finished = effectCoverUp();
                break;
            case PEF_UNCOVER_UP:
                m_finished = effectUncoverUp();
                break;
            case PEF_COVER_LEFT:
                m_finished = effectCoverLeft();
                break;
            case PEF_UNCOVER_LEFT:
                m_finished = effectUncoverLeft();
                break;
            case PEF_COVER_RIGHT:
                m_finished = effectCoverRight();
                break;
            case PEF_UNCOVER_RIGHT:
                m_finished = effectUncoverRight();
                break;
            case PEF_COVER_LEFT_UP:
                m_finished = effectCoverLeftUp();
                break;
            case PEF_UNCOVER_LEFT_UP:
                m_finished = effectUncoverLeftUp();
                break;
            case PEF_COVER_LEFT_DOWN:
                m_finished = effectCoverLeftDown();
                break;
            case PEF_UNCOVER_LEFT_DOWN:
                m_finished = effectUncoverLeftDown();
                break;
            case PEF_COVER_RIGHT_UP:
                m_finished = effectCoverRightUp();
                break;
            case PEF_UNCOVER_RIGHT_UP:
                m_finished = effectUncoverRightUp();
                break;
            case PEF_COVER_RIGHT_DOWN:
                m_finished = effectCoverRightDown();
                break;
            case PEF_UNCOVER_RIGHT_DOWN:
                m_finished = effectUncoverRightDown();
                break;
            case PEF_DISSOLVE:
                m_finished = effectDissolve();
                break;
            case PEF_STRIPS_LEFT_UP:
                m_finished = effectStripesLeftUp();
                break;
            case PEF_STRIPS_LEFT_DOWN:
                m_finished = effectStripesLeftDown();
                break;
            case PEF_STRIPS_RIGHT_UP:
                m_finished = effectStripesRightUp();
                break;
            case PEF_STRIPS_RIGHT_DOWN:
                m_finished = effectStripesRigthDown();
                break;
            case PEF_MELTING:
                m_finished = effectMelting();
                break;
            case PEF_RANDOM:
            case PEF_LAST_MARKER:
                m_finished = effectCloseHorizontal();
                break;
        }
        ++m_effectStep;
    }
    return m_finished;
}


void KPPageEffects::finish()
{
    if ( !m_finished )
    {
        bitBlt( m_dst, 0, 0, &m_pageTo );
        m_finished = true;
    }
}


bool KPPageEffects::effectNone() const
{
    bitBlt( m_dst, 0, 0, &m_pageTo );
    return true;
}


bool KPPageEffects::effectCloseHorizontal()  const
{
    int h = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) );

    bool finished = false;
    if ( h >= m_height / 2 )
    {
        h = m_height / 2;
        finished = true;
    }

    bitBlt( m_dst, 0, 0, &m_pageTo, 0, m_height / 2 - h, m_width, h, Qt::CopyROP );
    bitBlt( m_dst, 0, m_height - h, &m_pageTo, 0, m_height / 2, m_width, h );

    kdDebug(33001) << "effectCloseHorizontal h = " << h << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectCloseVertical()  const
{
    int w = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) );

    bool finished = false;
    if ( w >= m_width / 2 )
    {
        w = m_width / 2;
        finished = true;
    }

    bitBlt( m_dst, 0, 0, &m_pageTo, m_width / 2 - w, 0, w, m_height );
    bitBlt( m_dst, m_width - w, 0, &m_pageTo, m_width / 2, 0, w, m_height );

    kdDebug(33001) << "effectCloseVertical w = " << w << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectCloseFromAllDirections()  const
{
    double fact = (double) m_height / (double) m_width;
    int h = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) * fact );
    int w = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) );

    bool finished = false;
    if ( h >= m_height / 2 )
    {
        h = m_height / 2;
    }
    if ( w >= m_width / 2 )
    {
        w = m_width / 2;
    }
    if ( h >= m_height / 2 && w >= m_width / 2 )
    {
        finished = true;
    }

    bitBlt( m_dst, 0, 0, &m_pageTo, m_width / 2 - w, m_height / 2 - h, w, h );
    bitBlt( m_dst, m_width - w, 0, &m_pageTo, m_width / 2, m_height / 2 - h, w, h );
    bitBlt( m_dst, 0, m_height - h, &m_pageTo, m_width / 2 - w, m_height / 2, w, h );
    bitBlt( m_dst, m_width - w, m_height - h, &m_pageTo, m_width / 2, m_height / 2, w, h );

    kdDebug(33001) << "effectCloseFromAllDirections h = " << h << " w = " << w << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectOpenHorizontal()  const
{
    int h = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) );

    bool finished = false;
    if ( h >= m_height / 2 )
    {
        h = m_height / 2;
        finished = true;
    }

    bitBlt( m_dst, 0, m_height / 2 - h, &m_pageTo, 0, 0, m_width, h );
    bitBlt( m_dst, 0, m_height / 2, &m_pageTo, 0, m_height - h, m_width, h );

    kdDebug(33001) << "effectOpenHorizontal h = " << h << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectOpenVertical()  const
{
    int w = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) );

    bool finished = false;
    if ( w >= m_width / 2 )
    {
        w = m_width / 2;
        finished = true;
    }

    bitBlt( m_dst, m_width / 2 - w, 0, &m_pageTo, 0, 0, w, m_height );
    bitBlt( m_dst, m_width / 2, 0, &m_pageTo, m_width - w, 0, w, m_height );

    kdDebug(33001) << "effectOpenVertical w = " << w << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectOpenFromAllDirections()  const
{
    double fact = (double) m_height / (double) m_width;
    int h = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) * fact );
    int w = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) );

    bool finished = false;
    if ( h >= m_height / 2 )
    {
        h = m_height / 2;
    }
    if ( w >= m_width / 2 )
    {
        w = m_width / 2;
    }
    if ( h >= m_height / 2 && w >= m_width / 2 )
    {
        finished = true;
    }

    bitBlt( m_dst, m_width / 2 - w, m_height / 2 - h, &m_pageTo, 0, 0, w, h );
    bitBlt( m_dst, m_width / 2, m_height / 2 - h, &m_pageTo, m_width - w, 0, w, h );
    bitBlt( m_dst, m_width / 2 - w, m_height / 2, &m_pageTo, 0, m_height - h, w, h );
    bitBlt( m_dst, m_width / 2, m_height / 2, &m_pageTo, m_width - w, m_height - h, w, h );

    kdDebug(33001) << "effectOpenFromAllDirections h = " << h << " w = " << w << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectInterlockingHorizontal1() const
{
    int stepWidth = (int)( 5.0 * (m_speed + 1) );
    int w = (int)( m_effectStep * stepWidth );

    bool finished = false;
    if ( w >= m_width  )
    {
        w = m_width;
        finished = true;
    }

    int h = m_height / 4;

    bitBlt( m_dst, w, 0, &m_pageTo, w, 0, stepWidth, h );
    bitBlt( m_dst, m_width - w, h, &m_pageTo, m_width - w, h, stepWidth, h );
    bitBlt( m_dst, w, 2 * h, &m_pageTo, w, 2 * h, stepWidth, h );
    bitBlt( m_dst, m_width - w, 3 * h, &m_pageTo, m_width - w, 3 * h, stepWidth, h );

    kdDebug(33001) << "effectInterlockingHorizontal1 w = " << w << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectInterlockingHorizontal2() const
{
    int stepWidth = (int)( 5.0 * (m_speed + 1) );
    int w = (int)( m_effectStep * stepWidth );

    bool finished = false;
    if ( w >= m_width  )
    {
        w = m_width;
        finished = true;
    }

    int h = m_height / 4;

    bitBlt( m_dst, m_width - w, 0, &m_pageTo, m_width - w, 0, stepWidth, h );
    bitBlt( m_dst, w, h, &m_pageTo, w, h, stepWidth, h );
    bitBlt( m_dst, m_width - w, 2 * h, &m_pageTo, m_width - w, 2 * h, stepWidth, h );
    bitBlt( m_dst, w, 3 * h, &m_pageTo, w, 3 * h, stepWidth, h );

    kdDebug(33001) << "effectInterlockingHorizontal2 w = " << w << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectInterlockingVertical1() const
{
    int stepHeight = (int)( 5.0 * (m_speed + 1) );
    int h = (int)( m_effectStep * stepHeight );

    bool finished = false;
    if ( h >= m_height )
    {
        h = m_height;
        finished = true;
    }

    int w = m_width / 4;

    bitBlt( m_dst, 0, h, &m_pageTo, 0, h, w, stepHeight );
    bitBlt( m_dst, w, m_height - h, &m_pageTo, w, m_height - h, w, stepHeight );
    bitBlt( m_dst, 2 * w, h, &m_pageTo, 2 * w, h, w, stepHeight );
    bitBlt( m_dst, 3 * w, m_height - h, &m_pageTo, 3 * w, m_height - h, w, stepHeight );

    kdDebug(33001) << "effectInterlockingVertical1 w = " << w << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectInterlockingVertical2() const
{
    int stepHeight = (int)( 5.0 * (m_speed + 1) );
    int h = (int)( m_effectStep * stepHeight );

    bool finished = false;
    if ( h >= m_height  )
    {
        h = m_height;
        finished = true;
    }

    int w = m_width / 4;

    bitBlt( m_dst, 0, m_height - h, &m_pageTo, 0, m_height - h, w, stepHeight );
    bitBlt( m_dst, w, h, &m_pageTo, w, h, w, stepHeight );
    bitBlt( m_dst, 2 * w, m_height - h, &m_pageTo, 2 * w, m_height - h, w, stepHeight );
    bitBlt( m_dst, 3 * w, h, &m_pageTo, 3 * w, h, w, stepHeight );

    kdDebug(33001) << "effectInterlockingVertical2 w = " << w << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectSurround1() const
{
    //int stepSize = (int)( 5.0 * (m_speed + 1) );
    int stepSize = (int)( 10.0 * (m_speed + 1) );
    int step = m_effectStep * stepSize;

    int h = m_height / 10;
    int w = m_width / 10;
    int repaint_h = h;
    int repaint_w = w;

    bool finished = false;
    bool repaint= false;
    int rh = 0;
    int rw = 0;
    int repaint_rh = 0;
    int repaint_rw = 0;
    // 1
    if ( step < m_height )
    {
        rh = step;
        rw = 0;
        h = stepSize;
        if ( step + stepSize >= m_height )
        {
            repaint = true;

            repaint_rh = m_height - repaint_h;
            repaint_rw = step - m_height + repaint_w;
            repaint_w = stepSize;
        }
    }
    // 2
    else if ( step < m_height + m_width - w )
    {
        rh = m_height - h;
        rw = step - m_height + w;
        w = stepSize;
        if ( step + stepSize >= m_height + m_width - repaint_w )
        {
            repaint = true;

            repaint_rh = m_height - ( step - m_height + repaint_h - m_width + repaint_w + stepSize );
            repaint_rw = m_width - repaint_w;
            repaint_h = stepSize;
        }
    }
    // 3
    else if ( step < 2 * m_height - h + m_width - w )
    {
        rh = m_height - ( step - m_height + h - m_width + w + stepSize );
        rw = m_width - w;
        h = stepSize;
        if ( step + stepSize >=  2 * m_height - repaint_h + m_width - repaint_w )
        {
            repaint = true;

            repaint_rh = 0;
            repaint_rw = m_width - ( step - 2 * m_height + repaint_h - m_width + 2 * repaint_w + stepSize );
            repaint_w = stepSize;
        }
    }
    // 4
    else if ( step < 2 * m_height - h + 2 * m_width - 3 * w )
    {
        rh = 0;
        rw = m_width - ( step - 2 * m_height + h - m_width + 2 * w + stepSize );
        w = stepSize;
        if ( step + stepSize >= 2 * m_height - repaint_h + 2 * m_width - 3 * repaint_w )
        {
            repaint = true;

            repaint_rh = step - 2 * m_height + 2 * repaint_h - 2 * m_width + 3 * repaint_w;
            repaint_rw = repaint_w;
            repaint_h = stepSize;
        }
    }
    // 5
    else if ( step < 3 * m_height - 3 * h + 2 * m_width - 3 * w )
    {
        rh = step - 2 * m_height + 2 * h - 2 * m_width + 3 * w;
        rw = w;
        h = stepSize;
        if ( step + stepSize >=  3 * m_height - 3 * repaint_h + 2 * m_width - 3 * repaint_w )
        {
            repaint = true;

            repaint_rh = m_height - 2 * repaint_h;
            repaint_rw = step - 3 * m_height + 3 * repaint_h - 2 * m_width + 5 * repaint_w;
            repaint_w = stepSize;
        }
    }
    // 6
    else if ( step < 3 * m_height - 3 * h + 3 * m_width - 6 * w )
    {
        rh = m_height - 2 * h;
        rw = step - 3 * m_height + 3 * h - 2 * m_width + 5 * w;
        w = stepSize;
        if ( step + stepSize >= 3 * m_height - 3 * repaint_h + 3 * m_width - 6 * repaint_w )
        {
            repaint = true;

            repaint_rh = m_height - ( step - 3 * m_height + 5 * repaint_h - 3 * m_width + 6 * repaint_w + stepSize );
            repaint_rw = m_width - 2 * repaint_w;
            repaint_h = stepSize;
        }
    }
    // 7
    else if ( step < 4 * m_height - 6 * h + 3 * m_width - 6 * w )
    {
        rh = m_height - ( step - 3 * m_height + 5 * h - 3 * m_width + 6 * w + stepSize );
        rw = m_width - 2 * w;
        h = stepSize;
        if ( step + stepSize >= 4 * m_height - 6 * repaint_h + 3 * m_width - 6 * repaint_w )
        {
            repaint = true;

            repaint_rh = repaint_h;
            repaint_rw = m_width - ( step - 4 * m_height + 6 * repaint_h - 3 * m_width + 8 * repaint_w + stepSize );
            repaint_w = stepSize;
        }
    }
    // 8
    else if ( step < 4 * m_height - 6 * h + 4 * m_width - 10 * w )
    {
        rh = h;
        rw = m_width - ( step - 4 * m_height + 6 * h - 3 * m_width + 8 * w + stepSize );
        w = stepSize;
        if ( step + stepSize >= 4 * m_height - 6 * repaint_h + 4 * m_width - 10 * repaint_w )
        {
            repaint = true;

            repaint_rh = step - 4 * m_height + 8 * repaint_h - 4 * m_width + 10 * repaint_w;
            repaint_rw = 2 * repaint_w;
            repaint_h = stepSize;
        }
    }
    // 9
    else if ( step < 5 * m_height - 10 * h + 4 * m_width - 10 * w )
    {
        rh = step - 4 * m_height + 8 * h - 4 * m_width + 10 * w;
        rw = 2 * w;
        h = stepSize;
        if ( step + stepSize >= 5 * m_height - 10 * h + 4 * m_width - 10 * w )
        {
            repaint = true;

            repaint_rh = m_height - 3 * repaint_h;
            repaint_rw = step - 5 * m_height + 10 * repaint_h - 4 * m_width + 13 * repaint_w;
            repaint_w = stepSize;
        }
    }
    // 10
    else if ( step < 5 * m_height - 10 * h + 5 * m_width - 15 * w )
    {
        rh = m_height - 3 * h;
        rw = step - 5 * m_height + 10 * h - 4 * m_width + 13 * w;
        w = stepSize;
        if ( step + stepSize >= 5 * m_height - 10 * repaint_h + 5 * m_width - 15 * repaint_w )
        {
            repaint = true;

            repaint_rh = m_height - ( step - 5 * m_height + 13 * repaint_h - 5 * m_width + 15 * repaint_w + stepSize );
            repaint_rw = m_width - 3 * repaint_w;
            repaint_h = stepSize;
        }
    }
    // 11
    else if ( step < 6 * m_height - 15 * h + 5 * m_width - 15 * w )
    {
        rh = m_height - ( step - 5 * m_height + 13 * h - 5 * m_width + 15 * w + stepSize );
        rw = m_width - 3 * w;
        h = stepSize;
        if ( step + stepSize >= 6 * m_height - 15 * repaint_h + 5 * m_width - 15 * repaint_w )
        {
            repaint = true;

            repaint_rh = 2 * repaint_h;
            repaint_rw = m_width - ( step - 6 * m_height + 15 * repaint_h - 5 * m_width + 18 * repaint_w + stepSize );
            repaint_w = stepSize;
        }
    }
    // 12
    else if ( step < 6 * m_height - 15 * h + 6 * m_width - 21 * w )
    {
        rh = 2 * h;
        rw = m_width - ( step - 6 * m_height + 15 * h - 5 * m_width + 18 * w + stepSize );
        w = stepSize;
        if ( step + stepSize >= 6 * m_height - 15 * repaint_h + 6 * m_width - 21 * repaint_w )
        {
            repaint = true;

            repaint_rh = step - 6 * m_height + 18 * repaint_h - 6 * m_width + 21 * repaint_w;
            repaint_rw = 3 * repaint_w;
            repaint_h = stepSize;
        }
    }
    // 13
    else if ( step < 7 * m_height - 21 * h + 6 * m_width - 21 * w )
    {
        rh = step - 6 * m_height + 18 * h - 6 * m_width + 21 * w;
        rw = 3 * w;
        h = stepSize;
        if ( step + stepSize >= 7 * m_height - 21 * repaint_h + 6 * m_width - 21 * repaint_w )
        {
            repaint = true;

            repaint_rh = m_height - 4 * repaint_h;
            repaint_rw = step - 7 * m_height + 21 * repaint_h - 6 * m_width + 25 * repaint_w;
            repaint_w = stepSize;
        }
    }
    // 14
    else if ( step < 7 * m_height - 21 * h + 7 * m_width - 28 * w )
    {
        rh = m_height - 4 * h;
        rw = step - 7 * m_height + 21 * h - 6 * m_width + 25 * w;
        w = stepSize;
        if ( step + stepSize >= 7 * m_height - 21 * repaint_h + 7 * m_width - 28 * repaint_w )
        {
            repaint = true;

            repaint_rh = m_height - ( step - 7 * m_height + 25 * repaint_h - 7 * m_width + 28 * repaint_w + stepSize );
            repaint_rw = m_width - 4 * repaint_w;
            repaint_h = stepSize;
        }
    }
    // 15
    else if ( step < 8 * m_height - 28 * h + 7 * m_width - 28 * w )
    {
        rh = m_height - ( step - 7 * m_height + 25 * h - 7 * m_width + 28 * w + stepSize );
        rw = m_width - 4 * w;
        h = stepSize;
        if ( step + stepSize >= 8 * m_height - 28 * repaint_h + 7 * m_width - 28 * repaint_w )
        {
            repaint = true;

            repaint_rh = 3 * repaint_h;
            repaint_rw = m_width - ( step - 8 * m_height + 28 * repaint_h - 7 * m_width + 32 * repaint_w + stepSize );
            repaint_w = stepSize;
        }
    }
    // 16
    else if ( step < 8 * m_height - 28 * h + 8 * m_width - 36 * w )
    {
        rh = 3 * h;
        rw = m_width - ( step - 8 * m_height + 28 * h - 7 * m_width + 32 * w + stepSize );
        w = stepSize;
        if ( step + stepSize >= 8 * m_height - 28 * repaint_h + 8 * m_width - 36 * repaint_w )
        {
            repaint = true;

            repaint_rh = step - 8 * m_height + 32 * repaint_h - 8 * m_width + 36 * repaint_w;
            repaint_rw = 4 * repaint_w;
            repaint_h = stepSize;
        }
    }
    // 17
    else if ( step < 9 * m_height - 36 * h + 8 * m_width - 36 * w )
    {
        rh = step - 8 * m_height + 32 * h - 8 * m_width + 36 * w;
        rw = 4 * w;
        h = stepSize;
        if ( step + stepSize >= 9 * m_height - 36 * repaint_h + 8 * m_width - 36 * repaint_w )
        {
            repaint = true;

            repaint_rh = m_height - 5 * repaint_h;
            repaint_rw = step - 9 * m_height + 36 * repaint_h - 8 * m_width + 41 * repaint_w;
            repaint_w = stepSize;
        }
    }
    // 18
    else if ( step < 9 * m_height - 36 * h + 9 * m_width - 45 * w )
    {
        rh = m_height - 5 * h;
        rw = step - 9 * m_height + 36 * h - 8 * m_width + 41 * w;
        w = stepSize;
        if ( step + stepSize >= 9 * m_height - 36 * repaint_h + 9 * m_width - 45 * repaint_w )
        {
            repaint = true;

            repaint_rh = m_height - ( step - 9 * m_height + 41 * repaint_h - 9 * m_width + 45 * repaint_w + stepSize );
            repaint_rw = m_width - 5 * repaint_w;
            repaint_h = stepSize;
        }
    }
    // 19
    else if ( step < 10 * m_height - 45 * h + 9 * m_width - 45 * w )
    {
        rh = m_height - ( step - 9 * m_height + 41 * h - 9 * m_width + 45 * w + stepSize );
        rw = m_width - 5 * w;
        h = stepSize;
    }
    // 20
    /*else if ( step < 8 * m_height - 28 * h + 8 * m_width - 36 * w )
    {
        rh = 3 * h;
        rw = m_width - ( step - 8 * m_height + 28 * h - 7 * m_width + 32 * w + stepSize );
        w = stepSize;
    }*/
    else
    {
        finished = true;
    }

    bitBlt( m_dst, rw, rh, &m_pageTo, rw, rh, w, h );

    kdDebug(33001) << "effectSurround1 rh = " << rh << " rw = " << rw << " height = " << m_height << " m_width = "<< m_width
                   << " step =  " << step << endl;

    if ( repaint )
        bitBlt( m_dst, repaint_rw, repaint_rh, &m_pageTo, repaint_rw, repaint_rh, repaint_w, repaint_h );

    return finished;
}


bool KPPageEffects::effectFlyAway1() const
{
    return true;
}


bool KPPageEffects::effectBlindsHorizontal() const
{
    int stepHeight = (int)( 5.0 * (m_speed + 1) );
    int h = (int)( m_effectStep * stepHeight );
    int blockSize = m_height / 8;

    bool finished = false;
    if ( h >= blockSize )
    {
        h = blockSize;
        finished = true;
    }

    for ( int i=0; i < m_height; i += blockSize )
        bitBlt( m_dst, 0, h + i, &m_pageTo, 0, h + i, m_width, stepHeight );

    return finished;
}


bool KPPageEffects::effectBlindsVertical() const
{
    int stepWidth = (int)( 5.0 * (m_speed + 1) );
    int w = (int)( m_effectStep * stepWidth );
    int blockSize = m_width / 8;

    bool finished = false;
    if ( w >= blockSize )
    {
        w = blockSize;
        finished = true;
    }

    for ( int i=0; i < m_width; i += blockSize )
        bitBlt( m_dst, w + i, 0, &m_pageTo, w + i, 0, stepWidth, m_height );

    return finished;
}


bool KPPageEffects::effectBoxIn() const
{
    double fact = (double) m_height / (double) m_width;
    int stepHeigth = (int)( 5.0 * (m_speed + 1) * fact );
    int stepWidth = (int)( 5.0 * (m_speed + 1) );
    int h = (int)( m_effectStep * stepHeigth );
    int w = (int)( m_effectStep * stepWidth );

    bool finished = false;
    if ( h >= m_height / 2 )
    {
        h = m_height / 2;
    }
    if ( w >= m_width / 2 )
    {
        w = m_width / 2;
    }
    if ( h >= m_height / 2 && w >= m_width / 2 )
    {
        finished = true;
    }

    bitBlt( m_dst, w, h, &m_pageTo, w, h, m_width - 2 * w, stepHeigth );
    bitBlt( m_dst, w, h, &m_pageTo, w, h, stepWidth, m_height - 2 * h );
    bitBlt( m_dst, m_width - w, h, &m_pageTo, m_width - w, h, stepWidth, m_height - 2 * h );
    bitBlt( m_dst, w, m_height - h - stepHeigth, &m_pageTo, w, m_height - h - stepHeigth, m_width - 2 * w, stepHeigth );

    return finished;
}


bool KPPageEffects::effectBoxOut() const
{
    double fact = (double) m_height / (double) m_width;
    int stepHeigth = (int)( 5.0 * (m_speed + 1) * fact );
    int stepWidth = (int)( 5.0 * (m_speed + 1) );
    int h = (int)( m_effectStep * stepHeigth );
    int w = (int)( m_effectStep * stepWidth );

    bool finished = false;
    if ( h >= m_height / 2 )
    {
        h = m_height / 2;
    }
    if ( w >= m_width / 2 )
    {
        w = m_width / 2;
    }
    if ( h >= m_height / 2 && w >= m_width / 2 )
    {
        finished = true;
    }

    bitBlt( m_dst, m_width / 2 - w, m_height / 2 - h, &m_pageTo, m_width / 2 - w, m_height / 2 - h, 2 * w, stepHeigth );
    bitBlt( m_dst, m_width / 2 - w, m_height / 2 - h, &m_pageTo, m_width / 2 - w, m_height / 2 - h, stepWidth, 2 * h );
    bitBlt( m_dst, m_width / 2 + w, m_height / 2 - h, &m_pageTo, m_width / 2 + w, m_height / 2 - h, stepWidth, 2 * h );
    bitBlt( m_dst, m_width / 2 - w, m_height / 2 + h - stepHeigth, &m_pageTo, m_width / 2 - w, m_height / 2 + h - stepHeigth, w * 2 , stepHeigth );

    return finished;
}


bool KPPageEffects::effectCheckboardAcross() const
{
    int stepWidth = (int)( 5.0 * (m_speed + 1) );
    int w = (int)( m_effectStep * stepWidth );
    int blockSize = m_height / 8;

    bool finished = false;
    if ( w >= blockSize * 2 )
    {
        w = blockSize * 2;
        finished = true;
    }

    for ( int y = 0; y < m_height; y += blockSize )
    {
        int x = ( ( y / blockSize ) & 1 ) * blockSize;

        if ( x == blockSize && w >= blockSize - stepWidth )
            bitBlt( m_dst, w - blockSize, y, &m_pageTo, w - blockSize, y, stepWidth, blockSize );

        for ( ; x < m_width; x += 2 * blockSize )
        {
            bitBlt( m_dst, x + w, y, &m_pageTo, x + w, y, stepWidth, blockSize );
        }
    }

    return finished;
}


bool KPPageEffects::effectCheckboardDown() const
{
    int stepHeigth = (int)( 5.0 * (m_speed + 1) );
    int h = (int)( m_effectStep * stepHeigth );
    int blockSize = m_height / 8;

    bool finished = false;
    if ( h >= blockSize * 2 )
    {
        h = blockSize * 2;
        finished = true;
    }

    for ( int x = 0; x < m_width; x += blockSize )
    {
        int y = ( ( x / blockSize ) & 1 ) * blockSize;

        if ( y == blockSize && h >= blockSize - stepHeigth )
            bitBlt( m_dst, x, h - blockSize, &m_pageTo, x, h - blockSize, blockSize, stepHeigth );

        for ( ; y < m_width; y += 2 * blockSize )
        {
            bitBlt( m_dst, x, y + h, &m_pageTo, x, y + h, blockSize, stepHeigth );
        }
    }

    return finished;
}


bool KPPageEffects::effectCoverDown() const
{
    int h = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) );

    bool finished = false;
    if ( h >= m_height )
    {
        h = m_height;
        finished = true;
    }

    bitBlt( m_dst, 0, 0, &m_pageTo, 0, m_height - h, m_width, h );

    kdDebug(33001) << "effectCoverDown h = " << h << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectUncoverDown() const
{
    int stepHeigth = (int)( 5.0 * (m_speed + 1) );
    int h = (int)( m_effectStep * stepHeigth );

    bool finished = false;
    if ( h >= m_height )
    {
        h = m_height;
        finished = true;
    }

    bitBlt( m_dst, 0, h + stepHeigth, m_dst, 0, h, m_width, m_height - h - stepHeigth );
    bitBlt( m_dst, 0, h, &m_pageTo, 0, h, m_width, stepHeigth );

    kdDebug(33001) << "effectUncoverDown h = " << h << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectCoverUp() const
{
    int h = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) );

    bool finished = false;
    if ( h >= m_height )
    {
        h = m_height;
        finished = true;
    }

    bitBlt( m_dst, 0, m_height - h, &m_pageTo, 0, 0, m_width, h );

    kdDebug(33001) << "effectCoverUp h = " << h << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectUncoverUp()
{
    if ( m_effectStep == 0 )
        bitBlt( &m_pageFrom, 0, 0, m_dst );

    int stepHeigth = (int)( 5.0 * (m_speed + 1) );
    int h = (int)( m_effectStep * stepHeigth );

    bool finished = false;
    if ( h >= m_height )
    {
        h = m_height;
        finished = true;
    }

    bitBlt( m_dst, 0, 0, &m_pageFrom, 0, h + stepHeigth, m_width, m_height - h - stepHeigth );
    bitBlt( m_dst, 0, m_height - h - stepHeigth, &m_pageTo, 0, m_height - h - stepHeigth, m_width, stepHeigth );

    kdDebug(33001) << "effectUncoverUp h = " << h << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectCoverLeft() const
{
    int w = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) );

    bool finished = false;
    if ( w >= m_width )
    {
        w = m_width;
        finished = true;
    }

    bitBlt( m_dst, m_width - w, 0, &m_pageTo, 0, 0, w, m_height );

    kdDebug(33001) << "effectCoverLeft w = " << w << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectUncoverLeft()
{
    if ( m_effectStep == 0 )
        bitBlt( &m_pageFrom, 0, 0, m_dst );

    int stepWidth = (int)( 5.0 * (m_speed + 1) );
    int w = (int)( m_effectStep * stepWidth );

    bool finished = false;
    if ( w >= m_width )
    {
        w = m_width;
        finished = true;
    }

    bitBlt( m_dst, 0, 0, &m_pageFrom, w + stepWidth, 0, m_width - w - stepWidth, m_height );
    bitBlt( m_dst, m_width - w - stepWidth, 0, &m_pageTo, m_width - w - stepWidth, 0, stepWidth, m_height );

    kdDebug(33001) << "effectUncoverDown w = " << w << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectCoverRight() const
{
    int w = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) );

    bool finished = false;
    if ( w >= m_width )
    {
        w = m_width;
        finished = true;
    }

    bitBlt( m_dst, 0, 0, &m_pageTo, m_width - w, 0, w, m_height );

    kdDebug(33001) << "effectCoverLeft w = " << w << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectUncoverRight()
{
    if ( m_effectStep == 0 )
        bitBlt( &m_pageFrom, 0, 0, m_dst );

    int stepWidth = (int)( 5.0 * (m_speed + 1) );
    int w = (int)( m_effectStep * stepWidth );

    bool finished = false;
    if ( w >= m_width )
    {
        w = m_width;
        finished = true;
    }

    bitBlt( m_dst, 0, 0, &m_pageFrom, w + stepWidth, 0, m_width - w - stepWidth, m_height );
    bitBlt( m_dst, w, 0, &m_pageTo, w, 0, stepWidth, m_height );

    kdDebug(33001) << "effectUncoverDown w = " << w << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectCoverLeftUp() const
{
    double fact = (double) m_height / (double) m_width;
    int h = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) * fact );
    int w = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) );

    bool finished = false;
    if ( h >= m_height )
    {
        h = m_height;
    }
    if ( w >= m_width )
    {
        w = m_width;
    }
    if ( h >= m_height && w >= m_width )
    {
        finished = true;
    }

    bitBlt( m_dst, m_width - w, m_height - h, &m_pageTo, 0, 0, w, h );

    kdDebug(33001) << "effectCoverLeftUp h = " << h << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectUncoverLeftUp()
{
    if ( m_effectStep == 0 )
        bitBlt( &m_pageFrom, 0, 0, m_dst );

    double fact = (double) m_height / (double) m_width;
    int h = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) * fact );
    int w = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) );

    bool finished = false;
    if ( h >= m_height )
    {
        h = m_height;
    }
    if ( w >= m_width )
    {
        w = m_width;
    }
    if ( h >= m_height && w >= m_width )
    {
        finished = true;
    }

    bitBlt( m_dst, 0, 0, &m_pageFrom, w, h, m_width - w, m_height - h );
    bitBlt( m_dst, m_width - w, 0, &m_pageTo, m_width - w, 0, w, m_height );
    bitBlt( m_dst, 0, m_height - h, &m_pageTo, 0, m_height - h, m_width, h );

    kdDebug(33001) << "effectUncoverLeftUp h = " << h << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectCoverLeftDown() const
{
    double fact = (double) m_height / (double) m_width;
    int h = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) * fact );
    int w = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) );

    bool finished = false;
    if ( h >= m_height )
    {
        h = m_height;
    }
    if ( w >= m_width )
    {
        w = m_width;
    }
    if ( h >= m_height && w >= m_width )
    {
        finished = true;
    }

    bitBlt( m_dst, m_width - w, 0, &m_pageTo, 0, m_height - h, w, h );

    kdDebug(33001) << "effectCoverLeftDown h = " << h << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectUncoverLeftDown()
{
    if ( m_effectStep == 0 )
        bitBlt( &m_pageFrom, 0, 0, m_dst );

    double fact = (double) m_height / (double) m_width;
    int h = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) * fact );
    int w = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) );

    bool finished = false;
    if ( h >= m_height )
    {
        h = m_height;
    }
    if ( w >= m_width )
    {
        w = m_width;
    }
    if ( h >= m_height && w >= m_width )
    {
        finished = true;
    }

    // fix only copy what is nesseccary
    bitBlt( m_dst, 0, h, &m_pageFrom, w, 0, m_width - w, m_height - h );
    bitBlt( m_dst, 0, 0, &m_pageTo, 0, 0, m_width, h );
    bitBlt( m_dst, m_width - w, h, &m_pageTo, m_width - w, h, w, m_height - h );

    kdDebug(33001) << "effectUncoverLeftDown h = " << h << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectCoverRightUp() const
{
    double fact = (double) m_height / (double) m_width;
    int h = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) * fact );
    int w = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) );

    bool finished = false;
    if ( h >= m_height )
    {
        h = m_height;
    }
    if ( w >= m_width )
    {
        w = m_width;
    }
    if ( h >= m_height && w >= m_width )
    {
        finished = true;
    }

    bitBlt( m_dst, 0, m_height - h, &m_pageTo, m_width - w, 0, w, h );

    kdDebug(33001) << "effectCoverRightUp h = " << h << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectUncoverRightUp()
{
    if ( m_effectStep == 0 )
        bitBlt( &m_pageFrom, 0, 0, m_dst );

    double fact = (double) m_height / (double) m_width;
    int h = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) * fact );
    int w = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) );

    bool finished = false;
    if ( h >= m_height )
    {
        h = m_height;
    }
    if ( w >= m_width )
    {
        w = m_width;
    }
    if ( h >= m_height && w >= m_width )
    {
        finished = true;
    }
    // only draw what nesseccary
    bitBlt( m_dst, w, 0, &m_pageFrom, 0, h, m_width - w, m_height - h );
    bitBlt( m_dst, 0, 0, &m_pageTo, 0, 0, w, m_height );
    bitBlt( m_dst, w, m_height - h, &m_pageTo, w, m_height - h, m_width - w, h );

    kdDebug(33001) << "effectUncoverRightUp h = " << h << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectCoverRightDown() const
{
    double fact = (double) m_height / (double) m_width;
    int h = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) * fact );
    int w = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) );

    bool finished = false;
    if ( h >= m_height )
    {
        h = m_height;
    }
    if ( w >= m_width )
    {
        w = m_width;
    }
    if ( h >= m_height && w >= m_width )
    {
        finished = true;
    }

    bitBlt( m_dst, 0, 0, &m_pageTo, m_width - w, m_height - h, w, h );

    kdDebug(33001) << "effectCoverRightDown h = " << h << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectUncoverRightDown()
{
    if ( m_effectStep == 0 )
        bitBlt( &m_pageFrom, 0, 0, m_dst );

    double fact = (double) m_height / (double) m_width;
    int h = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) * fact );
    int w = (int)( 0.5 * m_effectStep * 10.0 * (m_speed + 1) );

    bool finished = false;
    if ( h >= m_height )
    {
        h = m_height;
    }
    if ( w >= m_width )
    {
        w = m_width;
    }
    if ( h >= m_height && w >= m_width )
    {
        finished = true;
    }
    // only draw what nesseccary
    bitBlt( m_dst, w, h, &m_pageFrom, 0, 0, m_width - w, m_height - h );
    bitBlt( m_dst, 0, 0, &m_pageTo, 0, 0, m_width, h );
    bitBlt( m_dst, 0, h, &m_pageTo, 0, h, w, m_height - h );

    kdDebug(33001) << "effectUncoverRightDown h = " << h << " height = " << m_height << " m_width = "<< m_width << endl;

    return finished;
}


bool KPPageEffects::effectDissolve() const
{
    return true;
}


bool KPPageEffects::effectStripesLeftUp() const
{
    int stepWidth = (int)( 5.0 * (m_speed + 1) );
    int wSteps = m_width / stepWidth;
    int hSteps = m_height / stepWidth + 1;

    int xStart = m_effectStep < wSteps ? m_effectStep : wSteps;
    int xStop = 1 > m_effectStep - hSteps + 1 ? 1 : m_effectStep - hSteps + 1;

    bool finished = false;
    if ( m_effectStep >= wSteps + hSteps )
    {
        finished = true;
    }

    int y = m_effectStep - wSteps + 1 > 1 ? m_effectStep - wSteps + 1 : 1;

    for ( int x = xStart; x >= xStop; --x )
    {
        bitBlt( m_dst, m_width - x * stepWidth, m_height - y * stepWidth,
                &m_pageTo, m_width - x * stepWidth, m_height - y * stepWidth , stepWidth, stepWidth );
        ++y;
    }

    return finished;
}


bool KPPageEffects::effectStripesLeftDown() const
{
    int stepWidth = (int)( 5.0 * (m_speed + 1) );
    int wSteps = m_width / stepWidth;
    int hSteps = m_height / stepWidth + 1;

    int xStart = m_effectStep < wSteps ? m_effectStep : wSteps;
    int xStop = 1 > m_effectStep - hSteps + 1 ? 1 : m_effectStep - hSteps + 1;

    bool finished = false;
    if ( m_effectStep >= wSteps + hSteps )
    {
        finished = true;
    }

    int y = m_effectStep - wSteps + 1 > 1 ? m_effectStep - wSteps + 1 : 1;

    for ( int x = xStart; x >= xStop; --x )
    {
        bitBlt( m_dst, m_width - x * stepWidth, ( y - 1 ) * stepWidth,
                &m_pageTo, m_width - x * stepWidth, ( y - 1 ) * stepWidth , stepWidth, stepWidth );
        ++y;
    }

    return finished;
}


bool KPPageEffects::effectStripesRightUp() const
{
    int stepWidth = (int)( 5.0 * (m_speed + 1) );
    int wSteps = m_width / stepWidth;
    int hSteps = m_height / stepWidth + 1;

    int xStart = m_effectStep < wSteps ? m_effectStep : wSteps;
    int xStop = 1 > m_effectStep - hSteps + 1 ? 1 : m_effectStep - hSteps + 1;

    bool finished = false;
    if ( m_effectStep >= wSteps + hSteps )
    {
        finished = true;
    }

    int y = m_effectStep - wSteps + 1 > 1 ? m_effectStep - wSteps + 1 : 1;

    for ( int x = xStart; x >= xStop; --x )
    {
        bitBlt( m_dst, ( x - 1 ) * stepWidth, m_height - y * stepWidth,
                &m_pageTo, ( x - 1 ) * stepWidth, m_height - y * stepWidth , stepWidth, stepWidth );
        ++y;
    }

    return finished;
}


bool KPPageEffects::effectStripesRigthDown() const
{
    int stepWidth = (int)( 5.0 * (m_speed + 1) );
    int wSteps = m_width / stepWidth;
    int hSteps = m_height / stepWidth + 1;

    int xStart = m_effectStep < wSteps ? m_effectStep : wSteps;
    int xStop = 1 > m_effectStep - hSteps + 1 ? 1 : m_effectStep - hSteps + 1;

    bool finished = false;
    if ( m_effectStep >= wSteps + hSteps )
    {
        finished = true;
    }

    int y = m_effectStep - wSteps + 1 > 1 ? m_effectStep - wSteps + 1 : 1;

    for ( int x = xStart; x >= xStop; --x )
    {
        bitBlt( m_dst, ( x - 1 ) * stepWidth, ( y - 1 ) * stepWidth,
                &m_pageTo, ( x - 1 ) * stepWidth, ( y - 1 ) * stepWidth , stepWidth, stepWidth );
        ++y;
    }

    return finished;
}


bool KPPageEffects::effectMelting() const
{
    return true;
}


