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

#ifndef pievaluecmd_h
#define pievaluecmd_h

#include <qlist.h>
#include <command.h>

class KPresenterDoc;
class KPObject;

/******************************************************************/
/* Class: PieValueCmd                                             */
/******************************************************************/

class PieValueCmd : public Command
{
public:
    struct PieValues
    {
        PieType pieType;
        int pieAngle, pieLength;
    };

    PieValueCmd( QString _name, QList<PieValues> &_oldValues, PieValues _newValues,
                 QList<KPObject> &_objects, KPresenterDoc *_doc );
    ~PieValueCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    PieValueCmd()
    {; }

    KPresenterDoc *doc;
    QList<PieValues> oldValues;
    QList<KPObject> objects;
    PieValues newValues;

};

#endif
