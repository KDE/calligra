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
/* Module: Lower/Raise Command                                    */
/******************************************************************/

#include "kpresenter_doc.h"
#include "lowraicmd.h"
#include "kpobject.h"

/******************************************************************/
/* Class: LowerRaiseCmd                                           */
/******************************************************************/

/*======================== constructor ===========================*/
LowerRaiseCmd::LowerRaiseCmd( QString _name, QList<KPObject> *_old, QList<KPObject> *__new, KPresenterDoc *_doc )
	: Command( _name )
{
	old = _old;
	_new = __new;
	old->setAutoDelete( false );
	_new->setAutoDelete( false );
	doc = _doc;

	for ( unsigned int i = 0; i < old->count(); i++ )
		old->at( i )->incCmdRef();
}

/*======================== destructor ============================*/
LowerRaiseCmd::~LowerRaiseCmd()
{
	for ( unsigned int i = 0; i < old->count(); i++ )
		old->at( i )->decCmdRef();

	// I'm not sure how to handle this here correctly ( to avoid memory leaks and not to crash... )
	//delete old;
	//delete _new;
}

/*====================== execute =================================*/
void LowerRaiseCmd::execute()
{
	doc->setObjectList( _new );
	doc->repaint( false );
}

/*====================== unexecute ===============================*/
void LowerRaiseCmd::unexecute()
{
	doc->setObjectList( old );
	doc->repaint( false );
}
