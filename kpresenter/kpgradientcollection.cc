#/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998			  */
/* Version: 0.1.0						  */
/* Author: Reginald Stadlbauer					  */
/* E-Mail: reggie@kde.org					  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* written for KDE (http://www.kde.org)				  */
/* KPresenter is under GNU GPL					  */
/******************************************************************/
/* Module: gradient collection					  */
/******************************************************************/

#include "kpgradientcollection.h"

#include <qstring.h>
#include <qpixmap.h>

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

