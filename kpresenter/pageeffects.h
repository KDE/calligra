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

#ifndef PAGEEFFECTS_H
#define PAGEEFFECTS_H

#include "global.h"


class QPaintDevice;
class QPixmap;


class KPPageEffects
{
public:
    /**
     * Class for displaying page effects. This class takes a paint device
     * and a pixmap and according to the effect it changes the paint device
     * to contain the pixmap.
     *
     * dst    The paint device to which the output will be directed.
     *        It has to show the current pixmap. 
     * pageTo The pixmap which will be shown when the effect is finished.
     * effect The effect which sould be used.
     * speed  The speed which should be used.
     */
    KPPageEffects( QPaintDevice *dst, const QPixmap &pageTo, PageEffect effect, PresSpeed speed );

    ~KPPageEffects();

    /**
     * doEffect handles the next step of the page effect. 
     * Returns true if the effect is finished.
     */
    bool doEffect();
    
    /**
     * Ends the page effect by displaying the m_pageTo.
     */
    void finish();
protected:
    bool effectNone();
    bool effectCloseHorizontal();
    bool effectCloseVertical();
    bool effectCloseFromAllDirections();
    bool effectOpenHorizontal();
    bool effectOpenVertical();
    bool effectOpenFromAllDirections();
    bool effectInterlockingHorizontal1();
    bool effectInterlockingHorizontal2();
    bool effectInterlockingVertical1();
    bool effectInterlockingVertical2();
    bool effectSurround1();
    bool effectFlyAway1();
    bool effectBlindsHorizontal();
    bool effectBlindsVertical();
    bool effectBoxIn();
    bool effectBoxOut();
    bool effectCheckboardAcross();
    bool effectCheckboardDown();
    bool effectCoverDown();
    bool effectUncoverDown();
    bool effectCoverUp();
    bool effectUncoverUp();
    bool effectCoverLeft();
    bool effectUncoverLeft();
    bool effectCoverRight();
    bool effectUncoverRight();
    bool effectCoverLeftUp();
    bool effectUncoverLeftUp();
    bool effectCoverLeftDown();
    bool effectUncoverLeftDown();
    bool effectCoverRightUp();
    bool effectUncoverRightUp();
    bool effectCoverRightDown();
    bool effectUncoverRightDown();
    bool effectDissolve();
    bool effectStripesLeftUp();
    bool effectStripesLeftDown();
    bool effectStripesRightUp();
    bool effectStripesRigthDown();
    bool effectMelting();

    QPaintDevice *m_dst;
    const QPixmap m_pageTo;
    QPixmap m_pageFrom;
    PageEffect m_effect;
    PresSpeed m_speed;
    int m_effectStep;
    int m_stepWidth;
    int m_stepHeight;
    int m_width;
    int m_height;
    bool m_finished;
};

#endif /* PAGEEFFECTS_H */
