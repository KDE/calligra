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
/* Module: Set Background Command (header)                        */
/******************************************************************/

#ifndef setbackcmd_h
#define setbackcmd_h

#include <qcolor.h>

#include "command.h"
#include "global.h"

class KPresenterDoc;

/******************************************************************/
/* Class: SetBackCmd                                              */
/******************************************************************/

class SetBackCmd : public Command
{
public:
	SetBackCmd( QString _name, QColor _backColor1, QColor _backColor2, BCType _bcType,
				QString _backPix, QString _backClip, BackView _backView, BackType _backType,
				QColor _oldBackColor1, QColor _oldBackColor2, BCType _oldBcType,
				QString _oldBackPix, QString _oldBackClip, BackView _oldBackView, BackType _oldBackType,
				bool _takeGlobal, int _currPgNum, KPresenterDoc *_doc );

	virtual void execute();
	virtual void unexecute();

protected:
	SetBackCmd()
    {; }

	QColor backColor1, backColor2;
	QString backPix, backClip;
	BCType bcType;
	BackView backView;
	BackType backType;
	QColor oldBackColor1, oldBackColor2;
	QString oldBackPix, oldBackClip;
	BCType oldBcType;
	BackView oldBackView;
	BackType oldBackType;
	bool takeGlobal;
	int currPgNum;
	KPresenterDoc *doc;

};

#endif
