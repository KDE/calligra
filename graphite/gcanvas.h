/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <trobin@kde.org>

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
#include <graphitepart.h>

class Ruler;
class GraphiteView;

class GCanvas : public QScrollView {

    Q_OBJECT

public:
    GCanvas(GraphiteView *view, GraphitePart *doc);
    virtual ~GCanvas() {}

    // Note: This has to be called immideately after constructtion, we rely on that
    // to avoid lots of if statements
    void setRulers(Ruler *hruler, Ruler *vruler);
    void showMousePos(bool show);

    void resizeContentsMM(const double &x, const double &y);

protected:
    virtual void contentsMousePressEvent(QMouseEvent *e) { m_doc->mousePressEvent(e, m_view); }
    virtual void contentsMouseReleaseEvent(QMouseEvent *e) { m_doc->mouseReleaseEvent(e, m_view); }
    virtual void contentsMouseDoubleClickEvent(QMouseEvent *e) { m_doc->mouseDoubleClickEvent(e, m_view); }
    virtual void contentsMouseMoveEvent(QMouseEvent *e);

    virtual void viewportPaintEvent(QPaintEvent *e);

    // ### don't forget to e->accept() key, dnd, and wheel events!!!
    virtual void keyPressEvent(QKeyEvent *e) { m_doc->keyPressEvent(e, m_view); }
    virtual void keyReleaseEvent(QKeyEvent *e) { m_doc->keyReleaseEvent(e, m_view); }

    virtual bool eventFilter(QObject *obj, QEvent *e);

private slots:
    void contentsMoving(int x, int y);

private:
    GraphitePart *m_doc;
    GraphiteView *m_view;
    Ruler *m_vertical, *m_horizontal;
    int m_eraseWidth, m_eraseHeight;
    bool m_haveFocus, m_gotFocus;
};

#endif // gcanvas_h
