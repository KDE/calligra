/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@mandrakesoft.com>

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

// The main widget (scroll area)

#ifndef gcanvas_h
#define gcanvas_h

#include <qscrollview.h>

class GraphiteView;
class GraphitePart;


class GCanvas : public QScrollView {

    Q_OBJECT
public:
    GCanvas(GraphiteView *view, GraphitePart *doc);
    virtual ~GCanvas();

protected:
    //virtual void viewportPaintEvent(QPaintEvent *e);
    // virtual void viewportResizeEvent(QResizeEvent *e);
    // virtual void viewportMousePressEvent(QMouseEvent *e);
    // virtual void viewportMouseReleaseEvent(QMouseEvent *e);
    // virtual void viewportMouseDoubleClickEvent(QMouseEvent *e);
    // virtual void viewportMouseMoveEvent(QMouseEvent *e);
private:
    GraphiteView *m_view;
    GraphitePart *m_doc;
};
#endif // gcanvas_h
