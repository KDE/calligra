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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrPageEffects.h"

#include <qpixmap.h>
#include <qwidget.h>
#include <qwmatrix.h>

#include <kapplication.h>
#include <kdebug.h>
#include <krandomsequence.h>


KPrPageEffects::KPrPageEffects( QPaintDevice *dst, const QPixmap &pageTo, PageEffect effect, EffectSpeed speed )
: m_dst( dst ), m_pageTo( pageTo ), m_pageFrom(m_pageTo.width(),m_pageTo.height()), m_effect(effect), m_speed(speed), m_effectStep(0)
, m_width(m_pageTo.width()), m_height(m_pageTo.height()), m_finished(false)
{
    if ( m_effect == PEF_RANDOM )
    {
        KRandomSequence random;
        m_randomEffect = static_cast<PageEffect>( random.getLong( PEF_LAST_MARKER ) );
    }
    int div[] = { 100, 65, 30 };

    m_stepWidth = (int) ( m_width / div[m_speed] );
    m_stepHeight = (int) ( m_height / div[m_speed] );
}


KPrPageEffects::~KPrPageEffects()
{
}


bool KPrPageEffects::doEffect()
{
    if ( !m_finished )
    {
        PageEffect effect = m_effect == PEF_RANDOM ? m_randomEffect : m_effect;
        switch ( effect )
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
            default:    
                m_finished = effectCloseHorizontal();
                break;
        }
        ++m_effectStep;
    }
    return m_finished;
}


void KPrPageEffects::finish()
{
    if ( !m_finished )
    {
        bitBlt( m_dst, 0, 0, &m_pageTo );
        m_finished = true;
    }
}


bool KPrPageEffects::effectNone() const
{
    bitBlt( m_dst, 0, 0, &m_pageTo );
    return true;
}


bool KPrPageEffects::effectCloseHorizontal()  const
{
    int h = m_effectStep * m_stepHeight;

    bool finished = false;
    if ( h >= m_height / 2 )
    {
        h = m_height / 2;
        finished = true;
    }

    bitBlt( m_dst, 0, 0, &m_pageTo, 0, m_height / 2 - h, m_width, h );
    bitBlt( m_dst, 0, m_height - h, &m_pageTo, 0, m_height / 2, m_width, h );

    return finished;
}


bool KPrPageEffects::effectCloseVertical()  const
{
    int w = m_effectStep * m_stepWidth;

    bool finished = false;
    if ( w >= m_width / 2 )
    {
        w = m_width / 2;
        finished = true;
    }

    bitBlt( m_dst, 0, 0, &m_pageTo, m_width / 2 - w, 0, w, m_height );
    bitBlt( m_dst, m_width - w, 0, &m_pageTo, m_width / 2, 0, w, m_height );

    return finished;
}


bool KPrPageEffects::effectCloseFromAllDirections()  const
{
    double fact = (double) m_height / (double) m_width;
    int h = (int)( m_effectStep * m_stepWidth * fact );
    int w = m_effectStep * m_stepWidth;

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

    return finished;
}


bool KPrPageEffects::effectOpenHorizontal()  const
{
    int h = m_effectStep * m_stepHeight;

    bool finished = false;
    if ( h >= m_height / 2 )
    {
        h = m_height / 2;
        finished = true;
    }

    bitBlt( m_dst, 0, m_height / 2 - h, &m_pageTo, 0, 0, m_width, h );
    bitBlt( m_dst, 0, m_height / 2, &m_pageTo, 0, m_height - h, m_width, h );

    return finished;
}


bool KPrPageEffects::effectOpenVertical()  const
{
    int w = m_effectStep * m_stepWidth;

    bool finished = false;
    if ( w >= m_width / 2 )
    {
        w = m_width / 2;
        finished = true;
    }

    bitBlt( m_dst, m_width / 2 - w, 0, &m_pageTo, 0, 0, w, m_height );
    bitBlt( m_dst, m_width / 2, 0, &m_pageTo, m_width - w, 0, w, m_height );

    return finished;
}


bool KPrPageEffects::effectOpenFromAllDirections()  const
{
    double fact = (double) m_height / (double) m_width;
    int h = (int)( m_effectStep * m_stepWidth * fact );
    int w = m_effectStep * m_stepWidth;

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

    return finished;
}


bool KPrPageEffects::effectInterlockingHorizontal1() const
{
    int w = m_effectStep * m_stepWidth;

    bool finished = false;
    if ( w >= m_width  )
    {
        w = m_width;
        finished = true;
    }

    int h = m_height / 4;

    bitBlt( m_dst, w, 0, &m_pageTo, w, 0, m_stepWidth, h );
    bitBlt( m_dst, m_width - w - m_stepWidth, h, &m_pageTo, m_width - w - m_stepWidth, h, m_stepWidth, h );
    bitBlt( m_dst, w, 2 * h, &m_pageTo, w, 2 * h, m_stepWidth, h );
    bitBlt( m_dst, m_width - w - m_stepWidth, 3 * h, &m_pageTo, m_width - w - m_stepWidth, 3 * h, m_stepWidth, m_height - 3 * h );

    return finished;
}


bool KPrPageEffects::effectInterlockingHorizontal2() const
{
    int w = m_effectStep * m_stepWidth;

    bool finished = false;
    if ( w >= m_width  )
    {
        w = m_width;
        finished = true;
    }

    int h = m_height / 4;

    bitBlt( m_dst, m_width - w - m_stepWidth, 0, &m_pageTo, m_width - w - m_stepWidth, 0, m_stepWidth, h );
    bitBlt( m_dst, w, h, &m_pageTo, w, h, m_stepWidth, h );
    bitBlt( m_dst, m_width - w - m_stepWidth, 2 * h, &m_pageTo, m_width - w - m_stepWidth, 2 * h, m_stepWidth, h );
    bitBlt( m_dst, w, 3 * h, &m_pageTo, w, 3 * h, m_stepWidth, h );

    return finished;
}


bool KPrPageEffects::effectInterlockingVertical1() const
{
    int h = m_effectStep * m_stepHeight;

    bool finished = false;
    if ( h >= m_height )
    {
        h = m_height;
        finished = true;
    }

    int w = m_width / 4;

    bitBlt( m_dst, 0, h, &m_pageTo, 0, h, w, m_stepHeight );
    bitBlt( m_dst, w, m_height - h - m_stepHeight, &m_pageTo, w, m_height - h - m_stepHeight, w, m_stepHeight );
    bitBlt( m_dst, 2 * w, h, &m_pageTo, 2 * w, h, w, m_stepHeight );
    bitBlt( m_dst, 3 * w, m_height - h - m_stepHeight, &m_pageTo, 3 * w, m_height - h - m_stepHeight, w, m_stepHeight );

    return finished;
}


bool KPrPageEffects::effectInterlockingVertical2() const
{
    int h = m_effectStep * m_stepHeight;

    bool finished = false;
    if ( h >= m_height  )
    {
        h = m_height;
        finished = true;
    }

    int w = m_width / 4;

    bitBlt( m_dst, 0, m_height - h - m_stepHeight, &m_pageTo, 0, m_height - h - m_stepHeight, w, m_stepHeight );
    bitBlt( m_dst, w, h, &m_pageTo, w, h, w, m_stepHeight );
    bitBlt( m_dst, 2 * w, m_height - h - m_stepHeight, &m_pageTo, 2 * w, m_height - h - m_stepHeight, w, m_stepHeight );
    bitBlt( m_dst, 3 * w, h, &m_pageTo, 3 * w, h, w, m_stepHeight );

    return finished;
}


bool KPrPageEffects::effectSurround1() const
{
    int div[] = { 20, 15, 10 };
    int stepSize = m_height / div[m_speed];
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
        if ( step + stepSize >= 5 * m_height - 10 * repaint_h + 4 * m_width - 10 * repaint_w )
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

    if ( repaint )
    {
        bitBlt( m_dst, repaint_rw, repaint_rh, &m_pageTo, repaint_rw, repaint_rh, repaint_w, repaint_h );
    }

    return finished;
}


bool KPrPageEffects::effectFlyAway1()
{
    bool finished = false;
    int steps[] = { 20, 15, 10 };
    int pSteps = steps[m_speed];

    if ( m_effectStep == 0 )
    {
        bitBlt( &m_pageFrom, 0, 0, m_dst );
        m_list.append( m_width );
        m_list.append( m_height );
        m_list.append( 0 );
        m_list.append( 0 );
    }
    else if ( m_effectStep <= pSteps )
    {
        double dw = 1.0 - 83.0 / 100.0 * m_effectStep / (double)pSteps;

        QWMatrix m;
        m.scale( dw, dw );
        QPixmap pix( m_pageFrom.xForm( m ) );

        if ( m_effectStep == pSteps )
            m_pageFrom = pix;
        
        int w = pix.width();
        int h = pix.height();
        int x = ( m_width - w ) / 2;
        int y = ( m_height - h ) / 2;

        int ow = *(m_list.at(0));
        int oh = *(m_list.at(1));
        int ox = *(m_list.at(2));
        int oy = *(m_list.at(3));
        
        bitBlt( m_dst, x, y, &pix, 0, 0 , w, h );
        // top
        bitBlt( m_dst, ox, oy, &m_pageTo, ox, oy, ow, y - oy );
        // left
        bitBlt( m_dst, ox, y, &m_pageTo, ox, y, x - ox, h );
        // right
        bitBlt( m_dst, x + w, y, &m_pageTo, x + w, y, ( ow - w + 1 ) / 2, h );
        // bottom
        bitBlt( m_dst, ox, y + h, &m_pageTo, ox, y + h, ow, ( oh - h + 1 ) / 2 );
        
        *(m_list.at(0)) = w;
        *(m_list.at(1)) = h;
        *(m_list.at(2)) = x;
        *(m_list.at(3)) = y;
    }
    else if ( m_effectStep <= 2 * pSteps )
    {
        int w = m_pageFrom.width();
        int h = m_pageFrom.height();
        int x = ( m_width - w ) / 2;
        int y = ( m_height - h ) / 2 - ( m_height - h ) / 2 * ( m_effectStep - pSteps ) / pSteps;

        int oy = *(m_list.at(3));

        bitBlt( m_dst, x, y, &m_pageFrom, 0, 0 , w, h );
        bitBlt( m_dst, x, y + h, &m_pageTo, x, y + h, w, oy - y);

        *(m_list.at(3)) = y;
    }
    else if ( m_effectStep <= 3 * pSteps )
    {
        int w = m_pageFrom.width();
        int h = m_pageFrom.height();
        int x = ( m_width - w ) / 2 - ( m_width - w ) / 2 * ( m_effectStep - 2 * pSteps ) / pSteps;
        int y = ( m_height - h ) / 2 * ( m_effectStep - 2 * pSteps ) / pSteps;

        int ox = *(m_list.at(2));
        int oy = *(m_list.at(3));

        bitBlt( m_dst, x, y, &m_pageFrom, 0, 0 , w, h );
        bitBlt( m_dst, ox, oy, &m_pageTo, ox, oy, w, y - oy);
        bitBlt( m_dst, x + w, oy, &m_pageTo, x + w, oy, x - ox, h );

        *(m_list.at(2)) = x;
        *(m_list.at(3)) = y;
    }
    else 
    {
        int w = m_pageFrom.width();
        int h = m_pageFrom.height();
        int x = ( m_width - w ) / 2 * ( m_effectStep - 3 * pSteps ) / pSteps;
        int y = ( m_height - h ) / 2 * ( m_effectStep - 2 * pSteps ) / pSteps;

        int ox = *(m_list.at(2));
        int oy = *(m_list.at(3));

        bitBlt( m_dst, x, y, &m_pageFrom, 0, 0 , w, h );
        bitBlt( m_dst, ox, oy, &m_pageTo, ox, oy, w, y - oy);
        bitBlt( m_dst, ox, oy, &m_pageTo, ox, oy, x - ox, h );

        if ( x >= m_height )
        {
            finished = true;
        }
        else
        {
            *(m_list.at(2)) = x;
            *(m_list.at(3)) = y;
        }
    }
      
    return finished;
}


bool KPrPageEffects::effectBlindsHorizontal() const
{
    int h = m_effectStep * m_stepHeight;
    int blockSize = m_height / 8;

    bool finished = false;
    if ( h >= blockSize )
    {
        h = blockSize;
        finished = true;
    }

    for ( int i=0; i < m_height; i += blockSize )
        bitBlt( m_dst, 0, h + i, &m_pageTo, 0, h + i, m_width, m_stepHeight );

    return finished;
}


bool KPrPageEffects::effectBlindsVertical() const
{
    int w = m_effectStep * m_stepWidth;
    int blockSize = m_width / 8;

    bool finished = false;
    if ( w >= blockSize )
    {
        w = blockSize;
        finished = true;
    }

    for ( int i=0; i < m_width; i += blockSize )
        bitBlt( m_dst, w + i, 0, &m_pageTo, w + i, 0, m_stepWidth, m_height );

    return finished;
}


bool KPrPageEffects::effectBoxIn() const
{
    double fact = (double) m_height / (double) m_width;
    int h = (int)( m_effectStep * m_stepWidth * fact );
    int stepHeight = (int)( ( m_effectStep + 1 ) * m_stepWidth * fact - h );
    int w = m_effectStep * m_stepWidth;

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

    // top
    bitBlt( m_dst, w, h, &m_pageTo, w, h, m_width - 2 * w, stepHeight );
    // left
    bitBlt( m_dst, w, h, &m_pageTo, w, h, m_stepWidth, m_height - 2 * h );
    //right
    bitBlt( m_dst, m_width - w, h, &m_pageTo, m_width - w, h, m_stepWidth, m_height - 2 * h );
    // bottom
    bitBlt( m_dst, w, m_height - h - stepHeight, &m_pageTo, w, m_height - h - stepHeight, m_width - 2 * w, stepHeight );

    return finished;
}


bool KPrPageEffects::effectBoxOut() const
{
    double fact = (double) m_height / (double) m_width;
    int h = (int)( m_effectStep * m_stepWidth * fact );
    int stepHeight = (int)( ( m_effectStep + 1 ) * m_stepWidth * fact - h );
    int w = m_effectStep * m_stepWidth;

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

    bitBlt( m_dst, m_width / 2 - w - m_stepWidth, m_height / 2 - h - stepHeight, 
            &m_pageTo, m_width / 2 - w - m_stepWidth, m_height / 2 - h - stepHeight, 2 * ( w + m_stepWidth ), stepHeight );
    bitBlt( m_dst, m_width / 2 - w - m_stepWidth, m_height / 2 - h, 
            &m_pageTo, m_width / 2 - w - m_stepWidth, m_height / 2 - h, m_stepWidth, 2 * h );
    bitBlt( m_dst, m_width / 2 + w, m_height / 2 - h, 
           &m_pageTo, m_width / 2 + w, m_height / 2 - h, m_stepWidth, 2 * h );
    bitBlt( m_dst, m_width / 2 - w - m_stepWidth, m_height / 2 + h, 
            &m_pageTo, m_width / 2 - w - m_stepWidth, m_height / 2 + h, 2 * ( w + m_stepWidth), stepHeight );

    return finished;
}


bool KPrPageEffects::effectCheckboardAcross() const
{
    int w = m_effectStep * m_stepWidth;
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

        if ( x == blockSize && w >= blockSize - m_stepWidth )
            bitBlt( m_dst, w - blockSize, y, &m_pageTo, w - blockSize, y, m_stepWidth, blockSize );

        for ( ; x < m_width; x += 2 * blockSize )
        {
            bitBlt( m_dst, x + w, y, &m_pageTo, x + w, y, m_stepWidth, blockSize );
        }
    }

    return finished;
}


bool KPrPageEffects::effectCheckboardDown() const
{
    int h = m_effectStep * m_stepHeight;
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

        if ( y == blockSize && h >= blockSize - m_stepHeight )
            bitBlt( m_dst, x, h - blockSize, &m_pageTo, x, h - blockSize, blockSize, m_stepHeight );

        for ( ; y < m_width; y += 2 * blockSize )
        {
            bitBlt( m_dst, x, y + h, &m_pageTo, x, y + h, blockSize, m_stepHeight );
        }
    }

    return finished;
}


bool KPrPageEffects::effectCoverDown() const
{
    int h = m_effectStep * m_stepHeight;

    bool finished = false;
    if ( h >= m_height )
    {
        h = m_height;
        finished = true;
    }

    bitBlt( m_dst, 0, 0, &m_pageTo, 0, m_height - h, m_width, h );

    return finished;
}


bool KPrPageEffects::effectUncoverDown() const
{
    int h = m_effectStep * m_stepHeight;

    bool finished = false;
    if ( h >= m_height )
    {
        h = m_height;
        finished = true;
    }

    bitBlt( m_dst, 0, h + m_stepHeight, m_dst, 0, h, m_width, m_height - h - m_stepHeight );
    bitBlt( m_dst, 0, h, &m_pageTo, 0, h, m_width, m_stepHeight );

    return finished;
}


bool KPrPageEffects::effectCoverUp() const
{
    int h = m_effectStep * m_stepHeight;

    bool finished = false;
    if ( h >= m_height )
    {
        h = m_height;
        finished = true;
    }

    bitBlt( m_dst, 0, m_height - h, &m_pageTo, 0, 0, m_width, h );

    return finished;
}


bool KPrPageEffects::effectUncoverUp()
{
    if ( m_effectStep == 0 )
        bitBlt( &m_pageFrom, 0, 0, m_dst );

    int h = m_effectStep * m_stepHeight;

    bool finished = false;
    if ( h >= m_height )
    {
        h = m_height;
        finished = true;
    }

    bitBlt( m_dst, 0, 0, &m_pageFrom, 0, h + m_stepHeight, m_width, m_height - h - m_stepHeight );
    bitBlt( m_dst, 0, m_height - h - m_stepHeight, &m_pageTo, 0, m_height - h - m_stepHeight, m_width, m_stepHeight );

    return finished;
}


bool KPrPageEffects::effectCoverLeft() const
{
    int w = m_effectStep * m_stepHeight;

    bool finished = false;
    if ( w >= m_width )
    {
        w = m_width;
        finished = true;
    }

    bitBlt( m_dst, m_width - w, 0, &m_pageTo, 0, 0, w, m_height );

    return finished;
}


bool KPrPageEffects::effectUncoverLeft()
{
    if ( m_effectStep == 0 )
        bitBlt( &m_pageFrom, 0, 0, m_dst );

    int w = m_effectStep * m_stepWidth;

    bool finished = false;
    if ( w >= m_width )
    {
        w = m_width;
        finished = true;
    }

    bitBlt( m_dst, 0, 0, &m_pageFrom, w + m_stepWidth, 0, m_width - w - m_stepWidth, m_height );
    bitBlt( m_dst, m_width - w - m_stepWidth, 0, &m_pageTo, m_width - w - m_stepWidth, 0, m_stepWidth, m_height );

    return finished;
}


bool KPrPageEffects::effectCoverRight() const
{
    int w = m_effectStep * m_stepWidth;

    bool finished = false;
    if ( w >= m_width )
    {
        w = m_width;
        finished = true;
    }

    bitBlt( m_dst, 0, 0, &m_pageTo, m_width - w, 0, w, m_height );

    return finished;
}


bool KPrPageEffects::effectUncoverRight()
{
    if ( m_effectStep == 0 )
        bitBlt( &m_pageFrom, 0, 0, m_dst );

    int w = m_effectStep * m_stepWidth;

    bool finished = false;
    if ( w >= m_width )
    {
        w = m_width;
        finished = true;
    }

    bitBlt( m_dst, w + m_stepWidth, 0, &m_pageFrom, 0, 0, m_width - w - m_stepWidth, m_height );
    bitBlt( m_dst, w, 0, &m_pageTo, w, 0, m_stepWidth, m_height );

    return finished;
}


bool KPrPageEffects::effectCoverLeftUp() const
{
    double fact = (double) m_height / (double) m_width;
    int h = (int)( m_effectStep * m_stepWidth * fact );
    int w = m_effectStep * m_stepWidth;

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

    return finished;
}


bool KPrPageEffects::effectUncoverLeftUp()
{
    if ( m_effectStep == 0 )
        bitBlt( &m_pageFrom, 0, 0, m_dst );

    double fact = (double) m_height / (double) m_width;
    int h = (int)( m_effectStep * m_stepWidth * fact );
    int w = m_effectStep * m_stepWidth;

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
    bitBlt( m_dst, 0, 0, &m_pageFrom, w, h, m_width - w, m_height - h );
    bitBlt( m_dst, m_width - w, 0, &m_pageTo, m_width - w, 0, w, m_height );
    bitBlt( m_dst, 0, m_height - h, &m_pageTo, 0, m_height - h, m_width, h );

    return finished;
}


bool KPrPageEffects::effectCoverLeftDown() const
{
    double fact = (double) m_height / (double) m_width;
    int h = (int)( m_effectStep * m_stepWidth * fact );
    int w = m_effectStep * m_stepWidth;

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

    return finished;
}


bool KPrPageEffects::effectUncoverLeftDown()
{
    if ( m_effectStep == 0 )
        bitBlt( &m_pageFrom, 0, 0, m_dst );

    double fact = (double) m_height / (double) m_width;
    int h = (int)( m_effectStep * m_stepWidth * fact );
    int w = m_effectStep * m_stepWidth;

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

    return finished;
}


bool KPrPageEffects::effectCoverRightUp() const
{
    double fact = (double) m_height / (double) m_width;
    int h = (int)( m_effectStep * m_stepWidth * fact );
    int w = m_effectStep * m_stepWidth;

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

    return finished;
}


bool KPrPageEffects::effectUncoverRightUp()
{
    if ( m_effectStep == 0 )
        bitBlt( &m_pageFrom, 0, 0, m_dst );

    double fact = (double) m_height / (double) m_width;
    int h = (int)( m_effectStep * m_stepWidth * fact );
    int w = m_effectStep * m_stepWidth;

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

    return finished;
}


bool KPrPageEffects::effectCoverRightDown() const
{
    double fact = (double) m_height / (double) m_width;
    int h = (int)( m_effectStep * m_stepWidth * fact );
    int w = m_effectStep * m_stepWidth;

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

    return finished;
}


bool KPrPageEffects::effectUncoverRightDown()
{
    if ( m_effectStep == 0 )
        bitBlt( &m_pageFrom, 0, 0, m_dst );

    double fact = (double) m_height / (double) m_width;
    int h = (int)( m_effectStep * m_stepWidth * fact );
    int w = m_effectStep * m_stepWidth;

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

    return finished;
}


bool KPrPageEffects::effectDissolve()
{
    KRandomSequence random;

    int blockSize = m_height / 32; // small enough
    int rowno = ( m_height + blockSize - 1 ) / blockSize;
    int colno = ( m_width + blockSize - 1 ) / blockSize;
    int cellno = rowno * colno;
  
    if ( m_effectStep == 0 )
    {
        for( int c = 0; c < cellno; c++ )
            m_list.append( c );
    }

    int steps[] = { 30, 60, 90 };
    int dissove = steps[m_speed];
    while ( !m_list.isEmpty() && dissove > 0 )
    {
        --dissove;

        int index = random.getLong( m_list.count() );
        QValueListIterator<int> it = m_list.at( index );
        
        unsigned int x = ( *it % colno ) * blockSize;
        unsigned int y = ( *it / colno ) * blockSize;

        m_list.remove( it );

        bitBlt( m_dst, x, y, &m_pageTo, x, y, blockSize, blockSize );
    }

    return m_list.isEmpty();
}


bool KPrPageEffects::effectStripesLeftUp() const
{
    int wSteps = m_width / m_stepWidth + 1;
    int hSteps = m_height / m_stepWidth + 1;

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
        bitBlt( m_dst, m_width - x * m_stepWidth, m_height - y * m_stepWidth,
                &m_pageTo, m_width - x * m_stepWidth, m_height - y * m_stepWidth , m_stepWidth, m_stepWidth );
        ++y;
    }

    return finished;
}


bool KPrPageEffects::effectStripesLeftDown() const
{
    int wSteps = m_width / m_stepWidth + 1;
    int hSteps = m_height / m_stepWidth + 1;

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
        bitBlt( m_dst, m_width - x * m_stepWidth, ( y - 1 ) * m_stepWidth,
                &m_pageTo, m_width - x * m_stepWidth, ( y - 1 ) * m_stepWidth , m_stepWidth, m_stepWidth );
        ++y;
    }

    return finished;
}


bool KPrPageEffects::effectStripesRightUp() const
{
    int wSteps = m_width / m_stepWidth + 1;
    int hSteps = m_height / m_stepWidth + 1;

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
        bitBlt( m_dst, ( x - 1 ) * m_stepWidth, m_height - y * m_stepWidth,
                &m_pageTo, ( x - 1 ) * m_stepWidth, m_height - y * m_stepWidth , m_stepWidth, m_stepWidth );
        ++y;
    }

    return finished;
}


bool KPrPageEffects::effectStripesRigthDown() const
{
    int wSteps = m_width / m_stepWidth + 1;
    int hSteps = m_height / m_stepWidth + 1;

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
        bitBlt( m_dst, ( x - 1 ) * m_stepWidth, ( y - 1 ) * m_stepWidth,
                &m_pageTo, ( x - 1 ) * m_stepWidth, ( y - 1 ) * m_stepWidth , m_stepWidth, m_stepWidth );
        ++y;
    }

    return finished;
}


bool KPrPageEffects::effectMelting()
{
    int count = 32;
    int max_melt = 2 * m_stepHeight;

    if ( m_effectStep == 0 )
    {
        bitBlt( &m_pageFrom, 0, 0, m_dst );

        for( int c = 0; c < count; c++ )
            m_list.append( 0 );
    }

    int w = ( m_width + count - 1 ) / count;

    QValueListIterator<int> it = m_list.begin();

    int finished = 32;
    for ( int c = 0; c < count; c++ )
    {
        int x = c * w;

        KRandomSequence random;
        int grow = 1 + random.getLong( max_melt );

        if ( *it + grow >= m_height )
        {
            grow = m_height - *it;
            --finished;
        }

        if( *it < m_height )
        {
            bitBlt( m_dst, x, *it, &m_pageTo, x, *it, w, grow );
            bitBlt( m_dst, x, *it + grow, &m_pageFrom, x, 0, w, m_height - *it - grow );

            *it += grow;
        }
        ++it;
    }
    
    return finished == 0;
}


