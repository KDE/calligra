// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2004 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef PBPREVIEW_H
#define PBPREVIEW_H

#include "global.h"

#include <qframe.h>
#include <qpen.h>

class KPGradient;
class KoZoomHandler;

/******************************************************************/
/* class Pen and Brush preview                                    */
/******************************************************************/

class PBPreview : public QFrame
{
    Q_OBJECT

public:
    enum PaintType {
        Pen,
        Brush,
        Gradient
    };

    PBPreview( QWidget* parent, const char* name, PaintType _paintType = Pen );
    ~PBPreview();
    void setPen( const QPen &_pen ) { pen = _pen; repaint( true ); }
    void setBrush( const QBrush &_brush ) { brush = _brush; }
    void setLineBegin( LineEnd lb ) { lineBegin = lb; repaint( true ); }
    void setLineEnd( LineEnd le ) { lineEnd = le; repaint( true ); }
    void setGradient( KPGradient *g ) { gradient = g; }
    void setPaintType( PaintType pt ) { paintType = pt; repaint(true);}

protected:
    void drawContents( QPainter *p );
    void resizeEvent( QResizeEvent *e );

private:
    PaintType paintType;
    QPen pen;
    QBrush brush;
    LineEnd lineBegin, lineEnd;
    KPGradient *gradient;
    KoZoomHandler *_zoomHandler;
};


#endif  /* PBPREVIEW_H */
