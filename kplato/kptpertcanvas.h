/* This file is part of the KDE project
   Copyright (C) 2003 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTPERTCANVAS_H
#define KPTPERTCANVAS_H

#include "kptnode.h"
#include "kptproject.h"

#include <qcanvas.h>
#include <qmemarray.h>
#include <qptrdict.h>

class KPTPertNodeItem;
class QTimer;
class QPainter;
class QPoint;
class QSize;

class KPTPertCanvas : public QCanvasView
{
    Q_OBJECT

public:
    KPTPertCanvas( QWidget *parent );
    virtual ~KPTPertCanvas();

    void draw(KPTProject& project);
    void clear();
    QSize canvasSize();

    KPTPertNodeItem *selectedItem();

    int verticalGap() { return m_verticalGap; }
    int horizontalGap() { return m_horizontalGap; }
    QSize itemSize() { return m_itemSize; }

    bool legalToLink(KPTNode &par, KPTNode &child);
    bool legalParents(KPTNode *par, KPTNode *child);
    bool legalChildren(KPTNode *par, KPTNode *child);

    void setColumn(int row, int col) { m_rows.at(row)[col] = true; }

    void mapNode(KPTPertNodeItem *item);
    void mapChildNode(KPTPertNodeItem *parentItem, KPTPertNodeItem *childItem, TimingRelation type);
    
    KPTNode *selectedNode();

protected:
    void drawRelations();

    void createChildItems(KPTPertNodeItem *node);
    KPTPertNodeItem *createNodeItem(KPTNode *node);

    void contentsMouseReleaseEvent ( QMouseEvent * e );

signals:
    void rightButtonPressed(KPTNode *node, const QPoint & point);
    void updateView(bool claculate);

private:
    QCanvas *m_canvas;

    QTimer *m_scrollTimer;
    bool m_mousePressed;
    bool m_printing;

	int m_verticalGap;
	int m_horizontalGap;
	QSize m_itemSize;

    QPtrDict<KPTPertNodeItem> m_nodes;
    QPtrList<KPTRelation> m_relations;

    QPtrList<QMemArray<bool> > m_rows;

#ifndef NDEBUG
    void printDebug( int );
#endif

};
#endif
