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

#ifndef penbrushcmd_h
#define penbrushcmd_h

#include <qlist.h>
#include <qpen.h>
#include <qbrush.h>

#include <command.h>

class KPresenterDoc;
class KPObject;

/******************************************************************/
/* Class: PenBrushCmd						  */
/******************************************************************/

class PenBrushCmd : public Command
{
public:
    struct Pen {
	QPen pen;
	LineEnd lineBegin, lineEnd;

	Pen &operator=( const Pen &_pen ) {
	    pen	 = _pen.pen;
	    lineBegin = _pen.lineBegin;
	    lineEnd = _pen.lineEnd;
	    return *this;
	}
    };

    struct Brush {
	QBrush brush;
	QColor gColor1;
	QColor gColor2;
	BCType gType;
	FillType fillType;
	bool unbalanced;
	int xfactor, yfactor;

	Brush &operator=( const Brush &_brush ) {
	    brush = _brush.brush;
	    gColor1 = _brush.gColor1;
	    gColor2 = _brush.gColor2;
	    gType = _brush.gType;
	    fillType = _brush.fillType;
	    unbalanced = _brush.unbalanced;
	    xfactor = _brush.xfactor;
	    yfactor = _brush.yfactor;
	    return *this;
	}
    };

    static const int LB_ONLY = 1;
    static const int LE_ONLY = 2;
    static const int PEN_ONLY = 4;
    static const int BRUSH_ONLY = 8;

    PenBrushCmd( QString _name, QList<Pen> &_oldPen, QList<Brush> &_oldBrush,
		 Pen _newPen, Brush _newBrush, QList<KPObject> &_objects, KPresenterDoc *_doc, int _flags = 0 );
    ~PenBrushCmd();

    virtual void execute();
    virtual void unexecute();

protected:
    PenBrushCmd()
    {; }

    KPresenterDoc *doc;
    QList<Pen> oldPen;
    QList<Brush> oldBrush;
    QList<KPObject> objects;
    Pen newPen;
    Brush newBrush;
    int flags;

};

#endif
