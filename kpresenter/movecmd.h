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

#ifndef movecmd_h
#define movecmd_h

#include <qlist.h>
#include <qpoint.h>

#include <command.h>

class KPresenterDoc;
class KPObject;

/******************************************************************/
/* Class: MoveByCmd                                               */
/******************************************************************/

class MoveByCmd : public Command
{
public:
    MoveByCmd( QString _name, QPoint _diff, QList<KPObject> &_objects, KPresenterDoc *_doc );
    ~MoveByCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    MoveByCmd()
    {; }

    QPoint diff;
    QList<KPObject> objects;
    KPresenterDoc *doc;

};

/******************************************************************/
/* Class: MoveByCmd2                                              */
/******************************************************************/

class MoveByCmd2 : public Command
{
public:
    MoveByCmd2( QString _name, QList<QPoint> &_diffs, QList<KPObject> &_objects, KPresenterDoc *_doc );
    ~MoveByCmd2();

    virtual void execute();
    virtual void unexecute();

protected:
    MoveByCmd2()
    {; }

    QList<QPoint> diffs;
    QList<KPObject> objects;
    KPresenterDoc *doc;

};

#endif
