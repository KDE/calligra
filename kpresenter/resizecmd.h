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

#ifndef resizecmd_h
#define resizecmd_h

#include <qpoint.h>
#include <qsize.h>

#include <command.h>

class KPresenterDoc;
class KPObject;

/******************************************************************/
/* Class: ResizeCmd                                               */
/******************************************************************/

class ResizeCmd : public Command
{
public:
    ResizeCmd( QString _name, QPoint _m_diff, QSize _r_diff, KPObject *_object, KPresenterDoc *_doc );
    ~ResizeCmd();

    virtual void execute();
    virtual void unexecute();
    virtual void unexecute( bool _repaint );

protected:
    ResizeCmd()
    {; }

    QPoint m_diff;
    QSize r_diff;
    KPObject *object;
    KPresenterDoc *doc;

};

#endif
