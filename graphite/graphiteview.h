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

#ifndef GRAPHITE_VIEW_H
#define GRAPHITE_VIEW_H

#include <koView.h>
#include <gcanvas.h>

class QResizeEvent;
class QMouseEvent;
class Ruler;
class KSelectAction;
class GraphitePart;

class GraphiteView : public KoView {

    Q_OBJECT

public:
    GraphiteView(GraphitePart *doc, QWidget *parent=0,
                 const char *name=0);
    virtual ~GraphiteView();

    virtual QWidget *canvas() { return m_canvas; }

    virtual int leftBorder() const { return 20; }
    virtual int topBorder() const { return 20; }

    // ###
    //virtual void canvasAddChild(KoViewChild *child);
    //virtual DCOPObject * dcopObject();
    //virtual void setupPrinter( QPrinter &printer );
    //virtual void print( QPrinter &printer );

public slots:
    void layoutChanged(const QRegion &diff);

protected slots:
    void slotViewZoom(const QString &text);

    void contentsMoving(int x, int y);
    void rulerUnitChanged(Graphite::Unit);
    void openPageLayoutDia();
    void borderChanged(const Graphite::PageBorders &b);

protected:
    void resizeEvent(QResizeEvent *e);

    virtual void updateReadWrite(bool readwrite);

private:
    void setupActions();
    void setupRulers();

    GraphitePart *m_doc;
    GCanvas *m_canvas;
    Ruler *m_vert, *m_horiz;
    int m_oldX, m_oldY;

    KSelectAction *m_zoomAction;
    KAction *file_pageLayout;
};

#endif
