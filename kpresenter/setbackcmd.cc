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

#include <kpresenter_doc.h>
#include <setbackcmd.h>
#include <kpbackground.h>

/******************************************************************/
/* Class: SetBackCmd						  */
/******************************************************************/

/*======================== constructor ===========================*/
SetBackCmd::SetBackCmd( QString _name, QColor _backColor1, QColor _backColor2, BCType _bcType,
			bool _backUnbalanced, int _backXFactor, int _backYFactor,
			const KoImageKey & _backPix, const KPClipartKey & _backClip,
                        BackView _backView, BackType _backType,
			QColor _oldBackColor1, QColor _oldBackColor2, BCType _oldBcType,
			bool _oldBackUnbalanced, int _oldBackXFactor, int _oldBackYFactor,
			const KPClipartKey & _oldBackPix, const KPClipartKey & _oldBackClip,
                        BackView _oldBackView, BackType _oldBackType,
			bool _takeGlobal, int _currPgNum, KPresenterDoc *_doc )
    : Command( _name ), backColor1( _backColor1 ), backColor2( _backColor2 ), unbalanced( _backUnbalanced ),
      xfactor( _backXFactor ), yfactor( _backYFactor ), backPix( _backPix ), backClip( _backClip ),
      oldBackColor1( _oldBackColor1 ), oldBackColor2( _oldBackColor2 ), oldUnbalanced( _oldBackUnbalanced ),
      oldXFactor( _oldBackXFactor ), oldYFactor( _oldBackYFactor ), oldBackPix( _oldBackPix ), oldBackClip( _oldBackClip )
{
    bcType = _bcType;
    backView = _backView;
    backType = _backType;
    oldBcType = _oldBcType;
    oldBackView = _oldBackView;
    oldBackType = _oldBackType;
    takeGlobal = _takeGlobal;
    currPgNum = _currPgNum;
    doc = _doc;
}

/*====================== execute =================================*/
void SetBackCmd::execute()
{
    if ( !takeGlobal ) {
	doc->setBackColor( currPgNum - 1, backColor1, backColor2, bcType,
			   unbalanced, xfactor, yfactor );
	doc->setBackType( currPgNum - 1, backType );
	doc->setBackView( currPgNum - 1, backView );
	doc->setBackPixmap( currPgNum - 1, backPix );
	doc->setBackClipart( currPgNum - 1, backClip );
	doc->restoreBackground( currPgNum - 1 );
    } else {
	unsigned int i = 0;
	for ( i = 0; i < doc->getPageNums(); i++ ) {
	    doc->setBackColor( i, backColor1, backColor2, bcType,
			       unbalanced, xfactor, yfactor );
	    doc->setBackType( i, backType );
	    doc->setBackView( i, backView );
	    doc->setBackPixmap( i, backPix );
	    doc->setBackClipart( i, backClip );
	}

	for ( i = 0; i < doc->getPageNums(); i++ )
	    doc->restoreBackground( i );
    }

    doc->repaint( false );
}

/*====================== unexecute ===============================*/
void SetBackCmd::unexecute()
{
    if ( !takeGlobal ) {
	doc->setBackColor( currPgNum - 1, oldBackColor1, oldBackColor2, oldBcType,
			   oldUnbalanced, oldXFactor, oldYFactor );
	doc->setBackType( currPgNum - 1, oldBackType );
	doc->setBackView( currPgNum - 1, oldBackView );
	doc->setBackPixmap( currPgNum - 1, oldBackPix );
	doc->setBackClipart( currPgNum - 1, oldBackClip );
	doc->restoreBackground( currPgNum - 1 );
    } else {
	unsigned int i = 0;
	for ( i = 0; i < doc->getPageNums(); i++ ) {
	    doc->setBackColor( i, oldBackColor1, oldBackColor2, oldBcType,
			       oldUnbalanced, oldXFactor, oldYFactor );
	    doc->setBackType( i, oldBackType );
	    doc->setBackView( i, oldBackView );
	    doc->setBackPixmap( i, oldBackPix );
	    doc->setBackClipart( i, oldBackClip );
	}

	for ( i = 0; i < doc->getPageNums(); i++ )
	    doc->restoreBackground( i );
    }

    doc->repaint( false );
}
