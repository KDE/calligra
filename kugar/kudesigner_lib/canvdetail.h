/* This file is part of the KDE project
   Copyright (C) 2003 Alexander Dymo <cloudtemple@mksat.net>

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
#ifndef CANVDETAIL_H
#define CANVDETAIL_H

#include "canvdefs.h"
#include "canvdetailbase.h"

class QCanvas;
class QPainter;

class CanvasDetail: public CanvasDetailBase{
public:
    CanvasDetail(int x, int y, int width, int height, int level, QCanvas * canvas);
    virtual int rtti() const { return KuDesignerRttiDetail; }
    virtual void draw(QPainter &painter);
    virtual QString getXml();
};

#endif
