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
/* Module: Set Effect Command                                     */
/******************************************************************/

#include "effectcmd.h"
#include "kpobject.h"

/******************************************************************/
/* Class: EffectCmd                                               */
/******************************************************************/

/*======================== constructor ===========================*/
EffectCmd::EffectCmd( QString _name, int _presNum, Effect _effect, Effect2 _effect2,
                      bool _disappear, Effect3 _effect3, int _disappearNum,
                      int _oldPresNum, Effect _oldEffect, Effect2 _oldEffect2,
                      bool _oldDisappear, Effect3 _oldEffect3, int _oldDisappearNum,
                      KPObject *_object )
    : Command( _name )
{
    presNum = _presNum;
    oldPresNum = _oldPresNum;
    effect = _effect;
    disappear = _disappear;
    effect3 = _effect3;
    disappearNum = _disappearNum;
    oldEffect = _oldEffect;
    effect2 = _effect2;
    oldEffect2 = _oldEffect2;
    oldDisappear = _oldDisappear;
    oldEffect3 = _oldEffect3;
    oldDisappearNum = _oldDisappearNum;
    object = _object;

    object->incCmdRef();
}

/*======================== destructor ============================*/
EffectCmd::~EffectCmd()
{
    object->decCmdRef();
}

/*====================== execute =================================*/
void EffectCmd::execute()
{
    object->setPresNum( presNum );
    object->setEffect( effect );
    object->setEffect2( effect2 );
    object->setDisappear( disappear );
    object->setEffect3( effect3 );
    object->setDisappearNum( disappearNum );
}

/*====================== unexecute ===============================*/
void EffectCmd::unexecute()
{
    object->setPresNum( oldPresNum );
    object->setEffect( oldEffect );
    object->setEffect2( oldEffect2 );
    object->setDisappear( oldDisappear );
    object->setEffect3( oldEffect3 );
    object->setDisappearNum( oldDisappearNum );
}

