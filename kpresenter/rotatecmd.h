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
/* Module: Rotate Command (header)                                */
/******************************************************************/

#ifndef rotatecmd_h
#define rotatecmd_h

#include <qlist.h>

#include "command.h"
#include "kpobject.h"

class KPresenterDoc;

/******************************************************************/
/* Class: RotateCmd                                               */
/******************************************************************/

class RotateCmd : public Command
{
public:
	struct RotateValues
	{
		float angle;
	};

	RotateCmd( QString _name, QList<RotateValues> &_oldRotate, float _newAngle,
			   QList<KPObject> &_objects, KPresenterDoc *_doc );
	~RotateCmd();

	virtual void execute();
	virtual void unexecute();

protected:
	RotateCmd()
    {; }

	KPresenterDoc *doc;
	QList<RotateValues> oldRotate;
	QList<KPObject> objects;
	float newAngle;

};

#endif
