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
               bool _disappear, Effect3 _effect3, int _disappearNum,
               int _oldPresNum, Effect _oldEffect, Effect2 _oldEffect2,
               bool _oldDisappear, Effect3 _oldEffect3, int _oldDisappearNum,
               KPObject *_object );
    ~EffectCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    EffectCmd()
    {; }

    int presNum, oldPresNum, disappearNum, oldDisappearNum;
    Effect effect, oldEffect;
    Effect2 effect2, oldEffect2;
    Effect3 effect3, oldEffect3;
    bool disappear, oldDisappear;
    KPObject *object;

};

#endif
