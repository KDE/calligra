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
/* Module: Set options Command (header)                           */
/******************************************************************/

#ifndef setoptionscmd_h
#define setoptionscmd_h

#include <kpoint.h>
#include <qcolor.h>

#include "command.h"
#include "kpobject.h"

class KPresenterDoc;

/******************************************************************/
/* Class: SetOptionsCmd                                           */
/******************************************************************/

class SetOptionsCmd : public Command
{
public:
	SetOptionsCmd( QString _name, QList<KPoint> &_diffs, QList<KPObject> &_objects,
				   int _rastX, int _rastY, int _orastX, int _orastY,
				   QColor _txtBackCol, QColor _otxtBackCol, KPresenterDoc *_doc );
	~SetOptionsCmd();

	virtual void execute();
	virtual void unexecute();

protected:
	SetOptionsCmd()
    {; }

	QList<KPoint> diffs;
	QList<KPObject> objects;
	int rastX, rastY;
	int orastX, orastY;
	KPresenterDoc *doc;
	QColor txtBackCol;
	QColor otxtBackCol;

};

#endif
