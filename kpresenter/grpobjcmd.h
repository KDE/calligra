/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef grpobjcmd_h
#define grpobjcmd_h

#include <qptrlist.h>

#include <kcommand.h>

class KPresenterDoc;
class KPGroupObject;
class KPObject;

/******************************************************************/
/* Class: GroupObjCmd						  */
/******************************************************************/

class GroupObjCmd : public KCommand
{
public:
    GroupObjCmd( const QString &_name,
		 const QPtrList<KPObject> &_objects,
		 KPresenterDoc *_doc );
    ~GroupObjCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    QPtrList<KPObject> objects;
    KPresenterDoc *doc;
    KPGroupObject *grpObj;

};

/******************************************************************/
/* Class: UnGroupObjCmd						  */
/******************************************************************/

class UnGroupObjCmd : public KCommand
{
public:
    UnGroupObjCmd( const QString &_name,
		 KPGroupObject *grpObj_,
		 KPresenterDoc *_doc );
    ~UnGroupObjCmd();

    virtual void execute();
    virtual void unexecute();

protected:

    QPtrList<KPObject> objects;
    KPresenterDoc *doc;
    KPGroupObject *grpObj;

};

#endif
