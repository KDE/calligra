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

#ifndef rectvaluecmd_h
#define rectvaluecmd_h

#include <qlist.h>
#include <command.h>

class KPresenterDoc;
class KPObject;

/******************************************************************/
/* Class: RectValueCmd                                            */
/******************************************************************/

class RectValueCmd : public Command
{
public:
    struct RectValues
    {
        int xRnd, yRnd;
    };

    RectValueCmd( QString _name, QList<RectValues> &_oldValues, RectValues _newValues,
                  QList<KPObject> &_objects, KPresenterDoc *_doc );
    ~RectValueCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    RectValueCmd()
    {; }

    KPresenterDoc *doc;
    QList<RectValues> oldValues;
    QList<KPObject> objects;
    RectValues newValues;

};

#endif
