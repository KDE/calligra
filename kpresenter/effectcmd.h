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

#include <qlist.h>
#include <qvaluelist.h>

#include "command.h"
#include "global.h"

class KPObject;

/******************************************************************/
/* Class: EffectCmd                                               */
/******************************************************************/

class EffectCmd : public Command
{
public:
    struct EffectStruct {
	int presNum, disappearNum;
	Effect effect;
	Effect2 effect2;
	Effect3 effect3;
	bool disappear;
    };
    
    EffectCmd( QString _name, const QList<KPObject> &_objs,
	       const QValueList<EffectStruct> &_oldEffects, EffectStruct _newEffect ); 
    ~EffectCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    EffectCmd()
    {; }
    
    QValueList<EffectStruct> oldEffects;
    EffectStruct newEffect;
    QList<KPObject> objs;

};

#endif
