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

#ifndef effectcmd_h
#define effectcmd_h

#include <qlist.h>
#include <qvaluelist.h>

#include <command.h>
#include <global.h>

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
