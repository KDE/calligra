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
/* Module: Move Command (header)                                  */
/******************************************************************/

#ifndef movecmd_h
#define movecmd_h

#include <kpoint.h>
#include <qlist.h>
#include <kpoint.h>

#include "command.h"
#include "kpobject.h"

class KPresenterDoc;

/******************************************************************/
/* Class: MoveByCmd                                               */
/******************************************************************/

class MoveByCmd : public Command
{
public:
	MoveByCmd( QString _name, KPoint _diff, QList<KPObject> &_objects, KPresenterDoc *_doc );
	~MoveByCmd();

	virtual void execute();
	virtual void unexecute();

protected:
	MoveByCmd()
    {; }

	KPoint diff;
	QList<KPObject> objects;
	KPresenterDoc *doc;

};

/******************************************************************/
/* Class: MoveByCmd2                                              */
/******************************************************************/

class MoveByCmd2 : public Command
{
public:
	MoveByCmd2( QString _name, QList<KPoint> &_diffs, QList<KPObject> &_objects, KPresenterDoc *_doc );
	~MoveByCmd2();

	virtual void execute();
	virtual void unexecute();

protected:
	MoveByCmd2()
    {; }

	QList<KPoint> diffs;
	QList<KPObject> objects;
	KPresenterDoc *doc;

};

#endif
