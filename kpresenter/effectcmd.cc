/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <effectcmd.h>
#include <kpobject.h>

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

