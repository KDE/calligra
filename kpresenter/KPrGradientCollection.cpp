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

#include "KPrGradientCollection.h"
#include "KPrGradient.h"
#include <QString>
//Added by qt3to4:
#include <QPixmap>

const QPixmap& KPrGradientCollection::getGradient( const QColor &_color1, const QColor &_color2,
                                                  BCType _bcType, const QSize &_size,
                                                  bool _unbalanced, int _xfactor, int _yfactor, bool addref )
{
    KPrGradient *cachedGradient = inGradientList( _color1, _color2, _bcType, _size, _unbalanced, _xfactor, _yfactor );

    if ( !cachedGradient ) {
        KPrGradient *kpgradient = new KPrGradient( _color1, _color2, _bcType, _unbalanced, _xfactor, _yfactor );
        kpgradient->setSize( _size );
        gradientList.append( kpgradient );

        if ( addref )
            kpgradient->addRef();

        return kpgradient->pixmap();
    } else {
        if ( addref )
            cachedGradient->addRef();

        return cachedGradient->pixmap();
    }
}

void KPrGradientCollection::removeRef( const QColor &_color1, const QColor &_color2,
                                      BCType _bcType, const QSize &_size,
                                      bool _unbalanced, int _xfactor, int _yfactor)
{
    KPrGradient* cachedGradient = inGradientList( _color1, _color2, _bcType, _size, _unbalanced, _xfactor, _yfactor );
    if ( cachedGradient ) {
        if ( cachedGradient->removeRef() ) // does deref say it was the last use?
            gradientList.removeRef( cachedGradient ); // yes -> remove from list
    }
}

KPrGradient* KPrGradientCollection::inGradientList( const QColor &_color1, const QColor &_color2,
                                                  BCType _bcType, const QSize &_size,
                                                  bool _unbalanced, int _xfactor, int _yfactor ) const
{
    Q3PtrListIterator<KPrGradient> it( gradientList );
    for ( ; it.current() ; ++it ) {
        KPrGradient *kpgradient = it.current();
        if ( kpgradient->getColor1() == _color1 && kpgradient->getColor2() == _color2 &&
             kpgradient->getBackColorType() == _bcType && kpgradient->size() == _size &&
             kpgradient->getUnbalanced() == _unbalanced && kpgradient->getXFactor() == _xfactor &&
             kpgradient->getYFactor() == _yfactor )
            return kpgradient;
    }
    return 0L;
}
