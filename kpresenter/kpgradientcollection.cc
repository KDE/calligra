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

/*======================= get Gradient ===========================*/
QPixmap* KPGradientCollection::getGradient( QColor _color1, QColor _color2, BCType _bcType, QSize _size,
					    bool _unbalanced, int _xfactor, int _yfactor, bool addref )
{
    int num = inGradientList( _color1, _color2, _bcType, _size, _unbalanced, _xfactor, _yfactor );

    if ( num == -1 ) {
	KPGradient *kpgradient = new KPGradient( _color1, _color2, _bcType, _size, _unbalanced, _xfactor, _yfactor );
	gradientList.append( kpgradient );

	if ( addref )
	    kpgradient->addRef();

	return kpgradient->getGradient();
    } else {
	if ( addref )
	    gradientList.at( num )->addRef();

	return gradientList.at( num )->getGradient();
    }
}

/*====================== remove ref =============================*/
void KPGradientCollection::removeRef( QColor _color1, QColor _color2, BCType _bcType, QSize _size,
				      bool _unbalanced, int _xfactor, int _yfactor)
{
    int num = inGradientList( _color1, _color2, _bcType, _size, _unbalanced, _xfactor, _yfactor );

    if ( num != -1 ) {
	if ( gradientList.at( num )->removeRef() ) {
	    gradientList.remove( num );

	}
    }
}

/*========================== in gradient list? ====================*/
int KPGradientCollection::inGradientList( QColor _color1, QColor _color2, BCType _bcType, QSize _size,
					  bool _unbalanced, int _xfactor, int _yfactor )
{
    if ( !gradientList.isEmpty() ) {
	KPGradient *kpgradient = 0;
	for ( int i = 0; i < static_cast<int>( gradientList.count() ); i++ ) {
	    kpgradient = gradientList.at( i );
	    if ( kpgradient->getColor1() == _color1 && kpgradient->getColor2() == _color2 &&
		 kpgradient->getBackColorType() == _bcType && kpgradient->getSize() == _size &&
		 kpgradient->getUnbalanced() == _unbalanced && kpgradient->getXFactor() == _xfactor &&
		 kpgradient->getYFactor() == _yfactor )
		return i;
	}
	return -1;
    }
    else return -1;
}
