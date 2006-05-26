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

#ifndef PAGEEFFECTS_H
#define PAGEEFFECTS_H

#include "global.h"
#include <q3valuelist.h>
//Added by qt3to4:
#include <QPixmap>


class QPaintDevice;
class QPixmap;


class KPrPageEffects
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
    KPrPageEffects( QPaintDevice *dst, const QPixmap &pageTo, PageEffect effect, EffectSpeed speed );

    ~KPrPageEffects();

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
    bool effectNone() const;
    bool effectCloseHorizontal() const;
    bool effectCloseVertical() const;
    bool effectCloseFromAllDirections() const;
    bool effectOpenHorizontal() const;
    bool effectOpenVertical() const;
    bool effectOpenFromAllDirections() const;
    bool effectInterlockingHorizontal1() const;
    bool effectInterlockingHorizontal2() const;
    bool effectInterlockingVertical1() const;
    bool effectInterlockingVertical2() const;
    bool effectSurround1() const;
    bool effectFlyAway1();
    bool effectBlindsHorizontal() const;
    bool effectBlindsVertical() const;
    bool effectBoxIn() const;
    bool effectBoxOut() const ;
    bool effectCheckboardAcross() const;
    bool effectCheckboardDown() const;
    bool effectCoverDown() const;
    bool effectUncoverDown() const;
    bool effectCoverUp() const;
    bool effectUncoverUp();
    bool effectCoverLeft() const;
    bool effectUncoverLeft();
    bool effectCoverRight() const;
    bool effectUncoverRight();
    bool effectCoverLeftUp() const;
    bool effectUncoverLeftUp();
    bool effectCoverLeftDown() const;
    bool effectUncoverLeftDown();
    bool effectCoverRightUp() const;
    bool effectUncoverRightUp();
    bool effectCoverRightDown() const;
    bool effectUncoverRightDown();
    bool effectDissolve();
    bool effectStripesLeftUp() const;
    bool effectStripesLeftDown() const;
    bool effectStripesRightUp() const;
    bool effectStripesRigthDown() const;
    bool effectMelting();

    QPaintDevice *m_dst;
    const QPixmap m_pageTo;
    QPixmap m_pageFrom;
    PageEffect m_effect;
    PageEffect m_randomEffect;
    EffectSpeed m_speed;
    Q3ValueList<int> m_list;
    int m_effectStep;
    int m_stepWidth;
    int m_stepHeight;
    int m_width;
    int m_height;
    bool m_finished;
};

#endif /* PAGEEFFECTS_H */
