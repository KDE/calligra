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

#ifndef deletecmd_h
#define deletecmd_h

#include <qlist.h>

#include <command.h>

class KPObject;
class KPresenterDoc;

/******************************************************************/
/* Class: DeleteCmd                                               */
/******************************************************************/

class DeleteCmd : public Command
{
public:
    DeleteCmd( QString _name, QList<KPObject> &_objects, KPresenterDoc *_doc );
    ~DeleteCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    DeleteCmd()
    {; }

    QList<KPObject> objects;
    KPresenterDoc *doc;

};

#endif
