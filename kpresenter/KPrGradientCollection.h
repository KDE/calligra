// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef kpgradientcollection_h
#define kpgradientcollection_h

#include <q3ptrlist.h>
//Added by qt3to4:
#include <QPixmap>
#include "global.h"
#include "KPrGradient.h"

class QPixmap;

/**
 * Class: KPrGradientCollection
 * Holds a collection of gradients, and is able to provide a gradient
 * for a given set of gradient parameters.
 * The idea is to be able to share gradients among pages which need the same one.
 * This class is currently only used for KPBackground, not for objects with gradients
 * (those having much less chances of sharing).
 */
class KPrGradientCollection
{
public:
    KPrGradientCollection()
        { gradientList.setAutoDelete( true ); }

    ~KPrGradientCollection()
        { gradientList.clear(); }

    /** Retrieve or create gradient for the given parameters */
    const QPixmap& getGradient( const QColor &_color1, const QColor &_color2, BCType _bcType, const QSize &_size,
                                bool _unbalanced, int _xfactor, int _yfactor, bool addref = true );

    /** Remove reference to gradient with the given parameters */
    void removeRef( const QColor &_color1, const QColor &_color2, BCType _bcType, const QSize &_size,
                    bool _unbalanced, int _xfactor, int _yfactor );

protected:
    KPrGradient* inGradientList( const QColor &_color1, const QColor &_color2, BCType _bcType, const QSize &_size,
                                bool _unbalanced, int _xfactor, int _yfactor ) const;

private:
    Q3PtrList<KPrGradient> gradientList;
};

#endif
