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
/* Module: Set Effect Command (header)                            */
/******************************************************************/

#ifndef effectcmd_h
#define effectcmd_h

#include "command.h"
#include "global.h"

class KPObject;

/******************************************************************/
/* Class: EffectCmd                                               */
/******************************************************************/

class EffectCmd : public Command
{
public:
	EffectCmd( QString _name, int _presNum, Effect _effect, Effect2 _effect2,
			   int _oldPresNum, Effect _oldEffect, Effect2 _oldEffect2,
			   KPObject *_object );
	~EffectCmd();

	virtual void execute();
	virtual void unexecute();

protected:
	EffectCmd()
    {; }

	int presNum, oldPresNum;
	Effect effect, oldEffect;
	Effect2 effect2, oldEffect2;
	KPObject *object;

};

#endif
