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

/*================================================================*/
EffectCmd::EffectCmd( QString _name, const QList<KPObject> &_objs,
		      const QValueList<EffectStruct> &_oldEffects, EffectStruct _newEffect ) 
    : Command( _name ), oldEffects( _oldEffects ),
      newEffect( _newEffect ), objs( _objs )
{
    for ( unsigned int i = 0; i < objs.count(); ++i )
        objs.at( i )->incCmdRef();
}

/*================================================================*/
EffectCmd::~EffectCmd()
{
    for ( unsigned int i = 0; i < objs.count(); ++i )
        objs.at( i )->decCmdRef();
}

/*================================================================*/
void EffectCmd::execute()
{
    KPObject *object = 0;
    for ( unsigned int i = 0; i < objs.count(); ++i ) {
	object = objs.at( i );
	
	object->setPresNum( newEffect.presNum );
	object->setEffect( newEffect.effect );
	object->setEffect2( newEffect.effect2 );
	object->setDisappear( newEffect.disappear );
	object->setEffect3( newEffect.effect3 );
	object->setDisappearNum( newEffect.disappearNum );
    }
}

/*================================================================*/
void EffectCmd::unexecute()
{
    KPObject *object = 0;
    for ( unsigned int i = 0; i < objs.count(); ++i ) {
	object = objs.at( i );
	
	object->setPresNum( oldEffects[ i ].presNum );
	object->setEffect( oldEffects[ i ].effect );
	object->setEffect2( oldEffects[ i ].effect2 );
	object->setDisappear( oldEffects[ i ].disappear );
	object->setEffect3( oldEffects[ i ].effect3 );
	object->setDisappearNum( oldEffects[ i ].disappearNum );
    }
}

