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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kpgradientcollection.h>
#include <kpgradient.h>
#include <qstring.h>

/******************************************************************/
/* Class: KPGradientCollection					  */
/******************************************************************/

/*======================= get gradient ===========================*/
const QPixmap& KPGradientCollection::getGradient( const QColor &_color1, const QColor &_color2, BCType _bcType, const QSize &_size,
					    bool _unbalanced, int _xfactor, int _yfactor, bool addref )
{
    KPGradient *cachedGradient = inGradientList( _color1, _color2, _bcType, _size, _unbalanced, _xfactor, _yfactor );

    if ( !cachedGradient ) {
	KPGradient *kpgradient = new KPGradient( _color1, _color2, _bcType, _unbalanced, _xfactor, _yfactor );
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

/*====================== remove ref =============================*/
void KPGradientCollection::removeRef( const QColor &_color1, const QColor &_color2, BCType _bcType, const QSize &_size,
				      bool _unbalanced, int _xfactor, int _yfactor)
{
    KPGradient* cachedGradient = inGradientList( _color1, _color2, _bcType, _size, _unbalanced, _xfactor, _yfactor );
    if ( cachedGradient ) {
	if ( cachedGradient->removeRef() ) // does deref say it was the last use?
	    gradientList.removeRef( cachedGradient ); // yes -> remove from list
    }
}

/*========================== in gradient list? ====================*/
KPGradient* KPGradientCollection::inGradientList( const QColor &_color1, const QColor &_color2, BCType _bcType, const QSize &_size,
                                                  bool _unbalanced, int _xfactor, int _yfactor ) const
{
    QPtrListIterator<KPGradient> it( gradientList );
    for ( ; it.current() ; ++it ) {
        KPGradient *kpgradient = it.current();
        if ( kpgradient->getColor1() == _color1 && kpgradient->getColor2() == _color2 &&
             kpgradient->getBackColorType() == _bcType && kpgradient->size() == _size &&
             kpgradient->getUnbalanced() == _unbalanced && kpgradient->getXFactor() == _xfactor &&
             kpgradient->getYFactor() == _yfactor )
            return kpgradient;
    }
    return 0L;
}
