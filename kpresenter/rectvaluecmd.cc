/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Rect Value Command                                     */
/******************************************************************/

#include "kpresenter_doc.h"
#include "rectvaluecmd.h"

#include <qcolor.h>

/******************************************************************/
/* Class: RectValueCmd                                            */
/******************************************************************/

/*======================== constructor ===========================*/
RectValueCmd::RectValueCmd( QString _name, QList<RectValues> &_oldValues, RectValues _newValues,
							QList<KPObject> &_objects, KPresenterDoc *_doc )
	: Command( _name ), oldValues( _oldValues ), objects( _objects )
{
	objects.setAutoDelete( false );
	oldValues.setAutoDelete( false );
	doc = _doc;
	newValues = _newValues;

	for ( unsigned int i = 0; i < objects.count(); i++ )
		objects.at( i )->incCmdRef();
}

/*======================== destructor ============================*/
RectValueCmd::~RectValueCmd()
{
	for ( unsigned int i = 0; i < objects.count(); i++ )
		objects.at( i )->decCmdRef();
	oldValues.setAutoDelete( true );
	oldValues.clear();
}

/*====================== execute =================================*/
void RectValueCmd::execute()
{
	for ( unsigned int i = 0; i < objects.count(); i++ )
		dynamic_cast<KPRectObject*>( objects.at( i ) )->setRnds( newValues.xRnd, newValues.yRnd );

	doc->repaint( false );
}

/*====================== unexecute ===============================*/
void RectValueCmd::unexecute()
{
	for ( unsigned int i = 0; i < objects.count(); i++ )
		dynamic_cast<KPRectObject*>( objects.at( i ) )->setRnds( oldValues.at( i )->xRnd, oldValues.at( i )->yRnd );

	doc->repaint( false );
}
