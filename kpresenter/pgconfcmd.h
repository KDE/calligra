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

#ifndef pgconfcmd_h
#define pgconfcmd_h

#include <command.h>

#include <qmap.h>

class KPresenterDoc;

/******************************************************************/
/* Class: PgConfCmd                                               */
/******************************************************************/

class PgConfCmd : public Command
{
public:
    PgConfCmd( QString _name, bool _manualSwitch, bool _infinitLoop,
               PageEffect _pageEffect, PresSpeed _presSpeed,
               bool _oldManualSwitch, bool _oldInfinitLoop,
               PageEffect _oldPageEffect, PresSpeed _oldPresSpeed,
               KPresenterDoc *_doc, int _pgNum );

    virtual void execute();
    virtual void unexecute();

protected:
    PgConfCmd()
    {; }

    bool manualSwitch, oldManualSwitch;
    bool infinitLoop, oldInfinitLoop;
    PageEffect pageEffect, oldPageEffect;
    PresSpeed presSpeed, oldPresSpeed;
    int pgNum;
    KPresenterDoc *doc;

};

#endif
