/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998		  */
/* Version: 0.1.0						  */
/* Author: Reginald Stadlbauer					  */
/* E-Mail: reggie@kde.org					  */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs			  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* written for KDE (http://www.kde.org)				  */
/* License: GNU GPL						  */
/******************************************************************/
/* Module: Set Background Command				  */
/******************************************************************/

#include "kpresenter_doc.h"
#include "setbackcmd.h"
#include "kpbackground.h"

/******************************************************************/
/* Class: SetBackCmd						  */
/******************************************************************/

/*======================== constructor ===========================*/
SetBackCmd::SetBackCmd( QString _name, QColor _backColor1, QColor _backColor2, BCType _bcType,
			bool _backUnbalanced, int _backXFactor, int _backYFactor,
			QString _backPix, QString _backClip, BackView _backView, BackType _backType,
			QColor _oldBackColor1, QColor _oldBackColor2, BCType _oldBcType,
			bool _oldBackUnbalanced, int _oldBackXFactor, int _oldBackYFactor,
			QString _oldBackPix, QString _oldBackClip, BackView _oldBackView, BackType _oldBackType,
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
	doc->setBackPixFilename( currPgNum - 1, backPix );
	doc->setBackClipFilename( currPgNum - 1, backClip );
	doc->restoreBackground( currPgNum - 1 );
    } else {
	unsigned int i = 0;
	for ( i = 0; i < doc->getPageNums(); i++ ) {
	    doc->setBackColor( i, backColor1, backColor2, bcType,
			       unbalanced, xfactor, yfactor );
	    doc->setBackType( i, backType );
	    doc->setBackView( i, backView );
	    doc->setBackPixFilename( i, backPix );
	    doc->setBackClipFilename( i, backClip );
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
	doc->setBackPixFilename( currPgNum - 1, oldBackPix );
	doc->setBackClipFilename( currPgNum - 1, oldBackClip );
	doc->restoreBackground( currPgNum - 1 );
    } else {
	unsigned int i = 0;
	for ( i = 0; i < doc->getPageNums(); i++ ) {
	    doc->setBackColor( i, oldBackColor1, oldBackColor2, oldBcType,
			       oldUnbalanced, oldXFactor, oldYFactor );
	    doc->setBackType( i, oldBackType );
	    doc->setBackView( i, oldBackView );
	    doc->setBackPixFilename( i, oldBackPix );
	    doc->setBackClipFilename( i, oldBackClip );
	}

	for ( i = 0; i < doc->getPageNums(); i++ )
	    doc->restoreBackground( i );
    }

    doc->repaint( false );
}

