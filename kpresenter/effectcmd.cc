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
                      int _oldPresNum, Effect _oldEffect, Effect2 _oldEffect2,
                      KPObject *_object )
    : Command( _name )
{
    presNum = _presNum;
    oldPresNum = _oldPresNum;
    effect = _effect;
    oldEffect = _oldEffect;
    effect2 = _effect2;
    oldEffect2 = _oldEffect2;
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
}

/*====================== unexecute ===============================*/
void EffectCmd::unexecute()
{
    object->setPresNum( oldPresNum );
    object->setEffect( oldEffect );
    object->setEffect2( oldEffect2 );
}

